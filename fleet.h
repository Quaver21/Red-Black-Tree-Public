/**
 * File:    fleet.cpp
 * Project: CMSC 341 Project 2 – The Fleet of Spaceships
 * Author:  Jay Buckwalter
 * Date:    5/17/22
 * Section: 03
 * E-mail:  rf29850@umbc.edu
 *
 * This file contains the declaration of the Fleet class and its nodes, Ships
 * A Fleet is a Red-Black Tree whose nodes simulate spaceships in a fleet
 */

#ifndef FLEET_H
#define FLEET_H
#include <iostream>
using namespace std;
class Grader;
class Tester;
enum STATE {ALIVE, LOST};
enum SHIPTYPE {CARGO, TELESCOPE, COMMUNICATOR, FUELCARRIER, ROBOCARRIER};
enum COLOR {RED, BLACK, DOUBLEBLACK};
const int MINID = 10000;
const int MAXID = 99999;
#define DEFAULT_ID 0
#define DEFAULT_TYPE CARGO
#define DEFAULT_STATE ALIVE
class Ship
{
    public:
        friend class Grader;
        friend class Tester;
        friend class Fleet;
        Ship(int id = DEFAULT_ID, SHIPTYPE type = DEFAULT_TYPE, STATE state = DEFAULT_STATE)
            : m_id(id), m_type(type), m_state(state)
        {
            m_left = nullptr;
            m_right = nullptr;
            m_color = RED;
        }
        int getID() const {return m_id;}
        STATE getState() const {return m_state;}
        string getStateStr() const
        {
            switch (m_state)
            {
                case LOST: return "LOST";
                case ALIVE: return "ALIVE";
                default: return "UNKNOWN";
            }
        }
        SHIPTYPE getType() const {return m_type;}
        string getTypeStr() const
        {
            switch (m_type)
            {
                case CARGO: return "CARGO";
                case TELESCOPE: return "TELESCOPE";
                case COMMUNICATOR: return "COMMUNICATOR";
                case FUELCARRIER: return "FUELCARRIER";
                case ROBOCARRIER: return "ROBOCARRIER";
                default: return "UNKNOWN";
            }
        }
        COLOR getColor() const {return m_color;}
        string getColorStr() const
        {
            switch (m_color)
            {
                case RED: return "RED";
                case BLACK: return "BLACK";
                case DOUBLEBLACK: return "DOUBLEBLACK";
                default: return "UNKNOWN";
            }
        }
        Ship* getLeft() const {return m_left;}
        Ship* getRight() const {return m_right;}
        void setID(const int id) {m_id = id;}
        void setState(STATE state) {m_state = state;}
        void setType(SHIPTYPE type) {m_type = type;}
        void setColor(COLOR color) {m_color = color;}
        void setLeft(Ship* left) {m_left = left;}
        void setRight(Ship* right) {m_right = right;}
    private:
        int m_id;
        SHIPTYPE m_type;
        STATE m_state;
        COLOR m_color;
        Ship* m_left;
        Ship* m_right;
};
class Fleet
{
    public:
        friend class Grader;
        friend class Tester;
        Fleet();
        ~Fleet();
        void clear();
        void insert(const Ship& ship);
        void remove(int id);
        void dumpTree() const;
        void listShips() const;
        bool setState(int id, STATE state);
        void removeLost();
        bool findShip(int id) const;
        Ship* getRoot() const {return m_root;}
    private:
        Ship* m_root;

        void dump(Ship* aShip) const;
        // ***************************************************
        // Any private helper functions must be delared here!
        // ***************************************************
        void deleteShip(Ship* aShip);
        Ship* recursInsert(Ship*& aShip, Ship* newShip, bool left);
        Ship* insertRebalance(Ship* grandparent, bool outerLeft, bool innerLeft);
        Ship* recursRemove(Ship*& aShip, int id, bool left);
        int replaceWithLargest(Ship* aShip);
        Ship* findLargest(Ship* aShip) const;
        Ship* removeRebalance(Ship* parent, bool left);
        Ship* getParent(Ship* aShip) const;
        Ship* lRotation(Ship* aShip);
        Ship* rRotation(Ship* aShip);
        void recolor(Ship* aShip);
        void recursList(Ship* aShip) const;
        bool recursRemoveLost(Ship* aShip);
};
#endif