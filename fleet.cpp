/**
 * File:    fleet.cpp
 * Project: CMSC 341 Project 2 – The Fleet of Spaceships
 * Author:  Jay Buckwalter
 * Date:    5/17/22
 * Section: 03
 * E-mail:  rf29850@umbc.edu
 *
 * This file contains the implementation of the Fleet class
 * A Fleet is a Red-Black Tree whose nodes simulate spaceships in a fleet
 */

#include "fleet.h"

// Name:    Fleet::Fleet (Default Constructor)
// Desc:    Default constructor for Fleet
// Precon:  None
// Postcon: An empty Fleet with no Ships will be created
Fleet::Fleet() : m_root(nullptr){}

// Name:    Fleet::~Fleet (Destructor)
// Desc:    Destructor for Fleet
// Precon:  None
// Postcon: All dynamically allocated memory will be deallocated
Fleet::~Fleet()
{
    deleteShip(m_root);
}

// Name:    Fleet::deleteShip
// Desc:    Recursively iterates through the subtree, deleting all Ships
// Precon:  None
// Postcon: The subtree aShip will be deleted
void Fleet::deleteShip(Ship* aShip)
{
    if(aShip != nullptr)
    {
        // Delete the left subtree
        deleteShip(aShip->m_left);
        // Delete the right subtree
        deleteShip(aShip->m_right);
        // Delete this Ship
        delete aShip;
    }
}

// Name:    Fleet::clear
// Desc:    Deallocates all memory and reinitializes member variable
// Precon:  None
// Postcon: this will be a default Fleet
void Fleet::clear()
{
    this->~Fleet();
    new (this) Fleet();
}

// Name:    Fleet::insert
// Desc:    Inserts a Ship into the Fleet
// Precon:  The Ship's id must be within [MINID, MAXID] and cannot already exist in the Fleet
//          Else does nothing
// Postcon: Fleet will be balanced and contain the new Ship
void Fleet::insert(const Ship& ship)
{
    // Check that the id to be inserted is valid
    if(ship.m_id >= MINID
        && ship.m_id <= MAXID
        && !findShip(ship.m_id))
    {
        Ship* newShip = new Ship(ship.m_id, ship.m_type, ship.m_state);
        // Special case: Inserting at the root
        if(m_root == nullptr)
        {
            m_root = newShip;
        }
        // Special Case: Inserting at root's child
        else if((ship.m_id < m_root->m_id ? m_root->m_left : m_root->m_right) == nullptr)
        {
            (ship.m_id < m_root->m_id ? m_root->m_left : m_root->m_right) = newShip;
        }
        // Normal insertion in root's child's subtree
        else
        {
            recursInsert(m_root, newShip, ship.m_id < m_root->m_id);
        }
        // Make sure the root is still BLACK (it might be RED)
        m_root->m_color = BLACK;
    }
}

// Name:    Fleet::recursInsert
// Desc:    Recursively iterates through the Fleet, looking for newShip's proper position
//          Rebalances the Fleet on the way back
// Precon:  newShip must not be nullptr
//          newShip's proper position must be in the subtrees of the passed Ship's children
// Postcon: Subtree whose root is aShip will be balanced and contain newShip
//          Returns the root of the current subtree
Ship* Fleet::recursInsert(Ship*& aShip, Ship* newShip, bool left)
{
    Ship*& possibility = (left ? aShip->m_left : aShip->m_right);
    // Base case, newShip's proper location found
    if(possibility == nullptr)
    {
        return newShip;
    }
    // Look in possibility's subtrees
    else
    {
        bool nextLeft = newShip->m_id < possibility->m_id;
        (nextLeft ? possibility->m_left : possibility->m_right) = recursInsert(possibility, newShip, nextLeft);
        aShip = insertRebalance(aShip, left, nextLeft);
    }
    return (left ? aShip->m_left : aShip->m_right);
}

// Name:    Fleet::insertRebalance
// Desc:    Rebalances a subtree containing a double RED
// Precon:  outerLeft and innerLeft must denote the positions of the possible double RED Ships, relative to the passed Ship
// Postcon: Subtree will be balanced to no longer contain a double RED
//          Returns the root of the current subtree
Ship* Fleet::insertRebalance(Ship* grandparent, bool outerLeft, bool innerLeft)
{
    // parent and child cannot be nullptr due to knowing the insertion route
    Ship* parent = (outerLeft ? grandparent->m_left : grandparent->m_right);
    Ship* child = (innerLeft ? parent->m_left : parent->m_right);
    // There is a double RED, rebalancing is necessary
    if(grandparent->m_color == BLACK
        && parent->m_color == RED
        && child->m_color == RED)
        {
        Ship* uncle = (outerLeft ? grandparent->m_right : grandparent->m_left);
        // uncle is RED, recoloring necessary
        if(uncle != nullptr
            && uncle->m_color == RED)
        {
            recolor(grandparent);
        }
        // uncle is BLACK or doesn't exist, rotation necessary
        else
        {
            // A double rotation is necessary
            if(outerLeft != innerLeft)
            {
                parent = (outerLeft ? grandparent->m_left : grandparent->m_right) = (innerLeft ? rRotation(parent) : lRotation(parent));
            }
            grandparent->m_color = RED;
            parent->m_color = BLACK;
            grandparent = (outerLeft ? rRotation(grandparent) : lRotation(grandparent));
        }
    }
    return grandparent;
}

// Name:    Fleet::remove
// Desc:    Removes a Ship whose id is passed in
// Precon:  There must exist Ship with the passed id
//          Else does nothing
// Postcon: The Fleet will be balanced and will not contain the Ship with the passed id
void Fleet::remove(int id)
{
    if(findShip(id))
    {
        // Normal removal
        if(m_root->m_id != id)
        {
            m_root = recursRemove(m_root, id, id < m_root->m_id);
        }
        // Special case: Removing root with a left child, replace the root with its largest left child and remove that child
        else if(m_root->m_left != nullptr)
        {
            m_root = recursRemove(m_root, replaceWithLargest(m_root), true);
        }
        // Special case: Removing root with only one child, replace root with child
        else if(m_root->m_right != nullptr)
        {
            Ship* temp = m_root;
            m_root = m_root->m_right;
            delete temp;
        }
        // Special case: Removing root with no children, delete root
        else
        {
            delete m_root;
            m_root = nullptr;
            return;
        }
        // Make sure the root is still BLACK (it might be DOUBLEBLACK)
        m_root->m_color = BLACK;
    }
}

// Name:    Fleet::recursRemove
// Desc:    Recursively iterates through the Fleet, looking for the Ship to be removed
//          Rebalances the Fleet on the way back
// Precon:  There must exist Ship with the passed id in the subtrees of the passed Ship
// Postcon: Subtree whose root is aShip will be balanced and will not contain the Ship with the passed id
//          Returns the root of the current subtree
Ship* Fleet::recursRemove(Ship*& aShip, int id, bool left)
{
    Ship*& possibility = (left ? aShip->m_left : aShip->m_right);
    bool nextLeft = true;
    // The Ship is in the right subtree of possibility
    if(id > possibility->m_id)
    {
        nextLeft = false;
    }
    // Found the Ship, check if it is a leaf
    else if(id == possibility->m_id)
    {
        // Ship is not a leaf, replace it with its largest left child
        if(possibility->m_left != nullptr)
        {
            id = replaceWithLargest(possibility);
        }
        // Ship is not a leaf, replace it with its smallest right child
        else if(possibility->m_right != nullptr)
        {
            possibility->m_left = possibility->m_right;
            possibility->m_right = nullptr;
            id = replaceWithLargest(possibility);
        }
        // Base case, possibility is the BLACK leaf Node to be deleted, make it DOUBLEBLACK and remove it
        else if(possibility->m_color == BLACK)
        {
            possibility->m_color = DOUBLEBLACK;
            Ship* toBeDeleted = possibility;
            // Rebalance the new DOUBLEBLACK
            Ship* temp = removeRebalance(aShip, left);
            // Get toBeDeleted's new parent
            Ship* parent = getParent(toBeDeleted);
            // Remove toBeDeleted from the tree
            (toBeDeleted == parent->m_left ? parent->m_left : parent->m_right) = nullptr;
            // Delete toBeDeleted
            delete toBeDeleted;
            return temp;
        }
        // Base case, possibility is the RED leaf Node to be deleted, remove it
        else
        {
            delete possibility;
            possibility = nullptr;
            return aShip;
        }
    }
    possibility = recursRemove(possibility, id, nextLeft);
    return removeRebalance(aShip, left);
}

// Name:    Fleet::findLargest
// Desc:    Replaces the Ship's data with the data of its largest left child
// Precon:  aShip and its left child must not be nullptr
// Postcon: Ship's data will instead be that of its largest left child
//          Returns the Ship's new id
int Fleet::replaceWithLargest(Ship* aShip)
{
    Ship* replacement = findLargest(aShip->m_left);
    // Replace the Ship's data with its replacement's data
    aShip->m_state = replacement->m_state;
    aShip->m_type = replacement->m_type;
    return aShip->m_id = replacement->m_id;
}

// Name:    Fleet::findLargest
// Desc:    Recursively iterates through the subtree, looking for the Ship with largest id
// Precon:  aShip must not be nullptr
// Postcon: Returns the Ship with largest id in the passed subtree
Ship* Fleet::findLargest(Ship* aShip) const
{
    // Base case, aShip has the largest id
    if(aShip->m_right == nullptr)
    {
        return aShip;
    }
    // Keep searching for the largest id
    else
    {
        return findLargest(aShip->m_right);
    }
}

// Name:    Fleet::removeRebalance
// Desc:    Rebalances a subtree containing a DOUBLEBLACK
// Precon:  left must denote the position of the possible DOUBLEBLACK Ship, relative to the passed Ship
// Postcon: Subtree will be balanced to no longer contain a DOUBLEBLACK
//          Returns the root of the current subtree
Ship* Fleet::removeRebalance(Ship* parent, bool left)
{
    Ship* child = (left ? parent->m_left : parent->m_right);
    // child is DOUBLEBLACK, rebalancing is necessary
    if(child != nullptr
        && child->m_color == DOUBLEBLACK)
    {
        // sibling cannot be nullptr due to having a BLACK sibling
        Ship* sibling = (left ? parent->m_right : parent->m_left);
        // sibling is RED, rotate it to be the parent and try again
        if(sibling->m_color == RED)
        {
            parent->m_color = RED;
            sibling->m_color = BLACK;
            // Rotate the sibling to be the parent and rebalance its child
            parent = (left ? lRotation(parent) : rRotation(parent));
            (left ? parent->m_left : parent->m_right) = removeRebalance((left ? parent->m_left : parent->m_right), left);
        }
        // sibling is BLACK and has a right RED child, make the child the same color as parent and rotate it up
        else if(sibling->m_right != nullptr
            && sibling->m_right->m_color == RED)
        {
            bool recolorNecessary = parent->m_color == RED
                && sibling->m_left != nullptr
                && sibling->m_left->m_color == RED;
            // Right-Right case
            if(left)
            {
                sibling->m_right->m_color = parent->m_color;
                parent = lRotation(parent);
            }
            // Left-Right case
            else
            {
                sibling->m_color = parent->m_color;
                sibling = parent->m_left = lRotation(sibling);
                sibling->m_color = BLACK;
                parent = rRotation(parent);
            }
            // Recolor if parent was originally RED and sibling originally had two RED children
            if(recolorNecessary)
            {
                recolor(parent);
            }
        }
        // sibling is BLACK and has a left RED child, make the child the same color as parent and rotate it up
        else if(sibling->m_left != nullptr
            && sibling->m_left->m_color == RED)
        {
            // Right-Left case
            if(left)
            {
                sibling->m_color = parent->m_color;
                sibling = parent->m_right = rRotation(sibling);
                sibling->m_color = BLACK;
                parent = lRotation(parent);
            }
            // Left-Left case
            else
            {
                sibling->m_left->m_color = parent->m_color;
                parent = rRotation(parent);
            }
        }
        // sibling is BLACK and has no RED children, recoloring is necessary
        else
        {
            // parent is BLACK, make it DOUBLEBLACK
            if(parent->m_color == BLACK)
            {
                parent->m_color = DOUBLEBLACK;
            }
            // parent is RED, make it BLACK
            else
            {
                parent->m_color = BLACK;
            }
            sibling->m_color = RED;
        }
        // child will longer be DOUBLEBLACK after this, make it BLACK
        child->m_color = BLACK;
    }
    return parent;
}

// Name:    Fleet::getParent
// Desc:    Searches for the parent of the passed Ship
// Precon:  Ship must be in the Fleet but not the root
// Postcon: Returns a pointer to the Ship's parent
Ship* Fleet::getParent(Ship* aShip) const
{
    // Iterate through the tree
    for(Ship* iter = m_root; true; iter = (iter->m_id >= aShip->m_id ? iter->m_left : iter->m_right))
    {
        // iter is aShip's parent, return it
        if(iter->m_left == aShip
            || iter->m_right == aShip)
        {
            return iter;
        }
    }
}

// Name:    Fleet::lRotation
// Desc:    Performs a left rotation around the passed Ship
// Precon:  aShip must not be nullptr
//          aShip's right child must not be nullptr
// Postcon: Subtree whose root is aShip will be rotated left
Ship* Fleet::lRotation(Ship* aShip)
{
    Ship* temp = aShip->m_right;
    aShip->m_right = temp->m_left;
    temp->m_left = aShip;
    return temp;
}

// Name:    Fleet::rRotation
// Desc:    Performs a right rotation around the passed Ship
// Precon:  aShip must not be nullptr
//          aShip's left child must not be nullptr
// Postcon: Subtree whose root is aShip will be rotated right
Ship* Fleet::rRotation(Ship* aShip)
{
    Ship* temp = aShip->m_left;
    aShip->m_left = temp->m_right;
    temp->m_right = aShip;
    return temp;
}

// Name:    Fleet::recolor
// Desc:    Passes a Ship's BLACK color to its children
// Precon:  aShip and its children must not be nullptr
// Postcon: The passed Ship will be BLACK and its children RED
void Fleet::recolor(Ship* aShip)
{
    aShip->m_color = RED;
    aShip->m_left->m_color = BLACK;
    aShip->m_right->m_color = BLACK;
}

// Name:    Fleet::dumpTree
// Desc:    Outputs an inorder visualization of the Fleet
// Precon:  None
// Postcon: Visualization of Fleet displayed to user
void Fleet::dumpTree() const
{
    dump(m_root);
}

// Name:    Fleet::dump
// Desc:    Recursively outputs an inorder visualization of the subtree whose root is aShip
// Precon:  None
// Postcon: Visualization of subtree whose root is aShip is displayed to user
void Fleet::dump(Ship* aShip) const
{
    if(aShip != nullptr)
    {
        cout << "(";
        // Dump the left child
        dump(aShip->m_left);
        // Dump this Ship
        cout << aShip->m_id << ":" << aShip->getColorStr();
        // Dump the right child
        dump(aShip->m_right);
        cout << ")";
    }
}

// Name:    Fleet::listShips
// Desc:    Outputs an inorder visualization of the Fleet
//          Shows each ship's m_id, m_state, and m_type
// Precon:  None
// Postcon: Visualization of Fleet displayed to user
void Fleet::listShips() const
{
    recursList(m_root);
}

// Name:    Fleet::recursList
// Desc:    Recursively outputs an inorder visualization of the subtree whose root is aShip
// Precon:  None
// Postcon: Visualization of subtree whose root is aShip is displayed to user
void Fleet::recursList(Ship* aShip) const
{
    if(aShip != nullptr)
    {
        // List the left child
        recursList(aShip->m_left);
        // List this Ship
        cout << aShip->m_id << ':' << aShip->getStateStr() << ':' << aShip->getTypeStr() << endl;
        // List the right child
        recursList(aShip->m_right);
    }
}

// Name:    Fleet::setState
// Desc:    Sets the state of the Ship with the passed id to be the passed state
// Precon:  Ship with the passed id must be in the Fleet
//          Else does nothing and returns false
// Postcon: Ship with the passed id will have m_state state
//          Returns true
bool Fleet::setState(int id, STATE state)
{
    // Iterate through the tree
    for(Ship* iter = m_root; iter != nullptr; iter = (iter->m_id > id ? iter->m_left : iter->m_right))
    {
        // Found the Ship
        if(iter->m_id == id)
        {
            iter->m_state = state;
            return true;
        }
    }
    // The Ship was never found
    return false;
}

// Name:    Fleet::removeLost
// Desc:    Removes all Ships whose m_state is LOST
// Precon:  None
// Postcon: Fleet will be balanced and will not contain any Ships with m_state LOST
void Fleet::removeLost()
{
    while(recursRemoveLost(m_root));
}

// Name:    Fleet::recursRemoveLost
// Desc:    Recursively removes each LOST Ship
// Precon:  None
// Postcon: If the subtree aShip has any LOST Ships, removes that Ship and returns true
//          Else returns false
bool Fleet::recursRemoveLost(Ship* aShip)
{
    // No lost Ships found
    if(aShip == nullptr)
    {
        return false;
    }
    // Lost Ship found, remove it
    else if(aShip->m_state == LOST)
    {
        remove(aShip->m_id);
        return true;
    }
    // Look for lost Ships in the subtrees
    else
    {
        return recursRemoveLost(aShip->m_left) || recursRemoveLost(aShip->m_right);
    }
}

// Name:    Fleet::findShip
// Desc:    Searches for a Ship with the passed id
// Precon:  None
// Postcon: If there is a Ship with the passed id, returns true
//          Else returns false
bool Fleet::findShip(int id) const
{
    // Iterate through the tree
    for(Ship* iter = m_root; iter != nullptr; iter = (iter->m_id > id ? iter->m_left : iter->m_right))
    {
        // Found the Ship
        if(iter->m_id == id)
        {
            return true;
        }
    }
    // The Ship was never found
    return false;
}