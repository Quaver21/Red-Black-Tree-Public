#include "fleet.h"
#include <math.h>
#include <time.h>
using namespace std;

const char BREAK[] = "*****************************************************************\n";

class Tester{
    public:
        Tester();
        int getTestCount();
        int getFailCount();
        void result(bool test);
        static bool insertTest(Fleet& fleet, Ship ships[], int size);
        static bool removeTest(Fleet& fleet, int ids[], int size);
        static bool setStateTest(Fleet& fleet, int id, STATE state = LOST);
        static bool removeLostTest(Fleet& fleet, int lostIds[], int size);
        static bool findShipTest(Fleet& fleet, int ids[], int size, bool answer);
        static bool insertTimeTest(int inputSize = 1000, int numTrials = 2, int numRepeats = 50);
        static bool removeTimeTest(int inputSize = 1000, int numTrials = 2, int numRepeats = 50);
        static bool findShipTimeTest(int inputSize = 1000, int numTrials = 2, int numRepeats = 50);
        static bool inArray(int item, int arr[], int size);
        static Fleet copyFleet(const Fleet& fleet);
    private:
        int m_testCount;
        int m_failCount;
        static Ship* copyShip(Ship* ship);
        static bool unbalanced(const Fleet& fleet);
        static int recursBalanced(Ship* ship);
        static bool fleetEqual(const Fleet& lhs, const Fleet& rhs);
        static bool shipEqual(Ship* lhs, Ship* rhs);
};

// Name:    Tester
// Desc:    Default constructor for Tester
// Precon:  None
// Postcon: A Tester with no tests done will be created
Tester::Tester() : m_testCount(0), m_failCount(0){}

// Name:    getTestCount
// Desc:    Accessor for m_testCount
// Precon:  None
// Postcon: Returns the value of m_testCount
int Tester::getTestCount() {return m_testCount;}

// Name:    getFailCount
// Desc:    Accessor for m_failCount
// Precon:  None
// Postcon: Returns the value of m_failCount
int Tester::getFailCount() {return m_failCount;}

// Name:    result
// Desc:    Interpreter for the result of a test
// Precon:  None
// Postcon: Adjusts the test and fail counts
//          Outputs to the user whether the test passed or failed
void Tester::result(bool test)
{
    m_testCount++;
    if(test)
    {
        cout << "\n        TEST PASSED\n\n";
    }
    else
    {
        m_failCount++;
        cout << "\n    ****TEST FAILED\n\n";
    }
}

// Name:    insertTest
// Desc:    Makes sure that insert successfully inserts all passed Ships
// Precon:  size denotes the size of the passed array
// Postcon: If any insertion fails, returns false
//          Else returns true
bool Tester::insertTest(Fleet& fleet, Ship ships[], int size)
{
    // Iterate through the passed Ships
    for(int i = 0; i < size; i++)
    {
        // Insertion shouldn't go through, make sure no change occurs
        if(fleet.findShip(ships[i].m_id)
            || ships[i].m_id < MINID
            || ships[i].m_id > MAXID)
        {
            Fleet copy = copyFleet(fleet);
            fleet.insert(ships[i]);
            // A change occured, return false
            if(!fleetEqual(fleet, copy))
            {
                return false;
            }
        }
        // Insertion should succeed, make sure it does
        else
        {
            fleet.insert(ships[i]);
            // The Ship wasn't inserted or the Fleet became unbalanced, return false
            if(!fleet.findShip(ships[i].m_id)
                || unbalanced(fleet))
            {
                return false;
            }
        }
    }
    // All insertions successful, return true
    return true;
}

// Name:    removeTest
// Desc:    Makes sure that remove successfully removes all passed ids
// Precon:  size denotes the size of the passed array
// Postcon: If any removal fails, returns false
//          Else returns true
bool Tester::removeTest(Fleet& fleet, int ids[], int size)
{
    // Iterate through the passed ids
    for(int i = 0; i < size; i++)
    {
        // Removal should succeed, make sure it does
        if(fleet.findShip(ids[i]))
        {
            fleet.remove(ids[i]);
            // The Ship was not removed or the Fleet became unbalanced, return false
            if(fleet.findShip(ids[i])
                    || unbalanced(fleet))
                    {
                return false;
            }
        }
        // Removal should not go through, make sure no change occurs
        else
        {
            Fleet copy = copyFleet(fleet);
            fleet.remove(ids[i]);
            // A change occurred, return false
            if(!fleetEqual(fleet, copy))
            {
                return false;
            }
        }
    }
    // All removals successful, return true
    return true;
}


// Name:    Tester::setStateTest
// Desc:    Makes sure that setState successfully changes the STATE of the Ship with the passed id
// Precon:  None
// Postcon: If setState successfully changes the STATE of the passed Ship id, returns true
//          Else returns false
bool Tester::setStateTest(Fleet& fleet, int id, STATE state)
{
    // The Ship is in the Fleet, make sure its STATE changes
    if(fleet.findShip(id))
    {
        Ship* iter = fleet.m_root;
        // Iterate to the passed id, in order to check that its STATE changes
        for(; iter->m_id != id; iter = (iter->m_id > id ? iter->m_left : iter->m_right));
        // The STATE was successfully changed, return true
        if(fleet.setState(id, state) == true
            && iter->m_state == state)
        {
            return true;
        }
    }
    // The Ship is not in the Fleet, make sure no change occurs
    else
    {
        Fleet copy = copyFleet(fleet);
        // No change occurred, return true
        if(fleet.setState(id, state) == false
            && fleetEqual(fleet, copy))
        {
            return true;
        }
    }
    // An error occurred, return false
    return false;
}

// Name:    Tester::removeLostTest
// Desc:    Makes sure that removeLost removes all lost Ships
// Precon:  lostIds must contain all lost ids in the Fleet
//          size denotes the size of the passed array
// Postcon: If every search matches the passed answer, returns true
//          Else returns false
bool Tester::removeLostTest(Fleet& fleet, int lostIds[], int size)
{
    // No lost ids, no change should occur
    if(size == 0)
    {
        Fleet copy = copyFleet(fleet);
        fleet.removeLost();
        // Change occured, return false
        if(!fleetEqual(fleet, copy))
        {
            return false;
        }
    }
    // There are lost ids, check that all are removed
    else
    {
        fleet.removeLost();
        // Return false if the fleet becomes unbalanced
        if(unbalanced(fleet))
        {
            return false;
        }
        // Iterate through the lost ids
        for(int i = 0; i < size; i++)
        {
            // Make sure none of the lost ids are in the Fleet
            if(fleet.findShip(lostIds[i]))
            {
                return false;
            }
        }
    }
    // All lost ids were successfully removed, return true
    return true;
}

// Name:    Tester::findShipTest
// Desc:    Makes sure that findShip successfully finds the passed Ship ids
// Precon:  If answer is true, ids contains ids in the Fleet
//          If answer is false, ids contains ids not in the Fleet
//          size denotes the side of the passed array
// Postcon: If every search matches the passed answer, returns true
//          Else returns false
bool Tester::findShipTest(Fleet& fleet, int ids[], int size, bool answer)
{
    // Iterate through the passed ids
    for(int i = 0; i < size; i++)
    {
        // Search for each id, return false if it doesn't return the right answer
        if(fleet.findShip(ids[i]) != answer)
        {
            return false;
        }
    }
    // Every search was successful, return true
    return true;
}

// Name:    Tester::insertTimeTest
// Desc:    Makes sure that the time taken for insert scales correctly with the size of the Fleet
// Precon:  inputSize should be a large positive number
//          numTrials must be > 1
//          numRepeats must be > 0
// Postcon: Returns true if the time scaling was correct between all trials
//          False otherwise
bool Tester::insertTimeTest(int inputSize, int numTrials, int numRepeats)
{
    const int inputScaling = 2;
    const double allowedVariability = .4;
    double expectedTimeScaling[numTrials - 1];
    // Fill expectedTimeScaling with the expected time scaling between trials
    for(int i = 0; i < numTrials - 1; i++)
    {
        // Summation from (j = 1) to (j = inputSize) of log2(j)
        int summationBefore = 0;
        for(int j = 1; j <= inputSize * pow(inputScaling, i); j++)
        {
            summationBefore += log2(j);
        }
        // Summation from (j = 1) to (j = inputSize * inputScaling) of log2(j)
        int summationAfter = summationBefore;
        for(int j = inputSize * pow(inputScaling, i) + 1; j <= inputSize * pow(inputScaling, i + 1); j++)
        {
            summationAfter += log2(j);
        }
        // Divide the summations in order to find the scaling
        expectedTimeScaling[i] = ((double) summationAfter) / summationBefore;
    }
    double timeDiff[numTrials];
    clock_t start, stop;
    // Repeat numTrials times
    for(int i = 0; i < numTrials; i++)
    {
        timeDiff[i] = 0;
        // Repeat each trial numRepeats times
        for(int j = 0; j < numRepeats; j++)
        {
            Fleet fleet;
            int ids[inputSize];
            Ship ships[inputSize];
            // Create enough Ships for inputSize insertions
            for(int k = 0; k < inputSize; k++)
            {
                do
                {
                    ids[k] = rand() % (MAXID - MINID + 1) + MINID;
                }
                while(inArray(ids[k], ids, k));
                ships[k] = Ship(ids[k], static_cast<SHIPTYPE>(rand() % 5), ALIVE);
            }
            // Time how long it takes to insert all Ships
            start = clock();
            for(int k = 0; k < inputSize; k++)
            {
                fleet.insert(ships[k]);
            }
            stop = clock();
            timeDiff[i] += stop - start;
        }
        // Average the time between trials
        timeDiff[i] /= numRepeats;
        // Output the results of the trial to the user
        cout << "\n\tInserting " << inputSize << " Ships into an empty Fleet took an average of "
             << timeDiff[i] << " clock ticks ("<< timeDiff[i]/CLOCKS_PER_SEC << " seconds)";
        // Increase the inputSize for the next trial
        inputSize *= inputScaling;
    }
    bool output = true;
    // Compare each trial to its neighbors (ie trial 0 to 1, then trial 1 to 2...)
    for(int i = 0; i < numTrials - 1; i++)
    {
        // Output the comparison of the trials to the user
        cout << "\n\tExpected Scaling from " << inputSize / pow(inputScaling, numTrials - i) << " to " << inputSize / pow(inputScaling, numTrials - i - 1)
             << ": " << expectedTimeScaling[i] << " ± .4, Actual Scaling: " << timeDiff[i + 1] / timeDiff[i];
        // Output false if the actual timeScaling between any trials wasn't within the range expectedTimeScaling ± allowedVariability
        if(timeDiff[i + 1] / timeDiff[i] > expectedTimeScaling[i] + allowedVariability
            || timeDiff[i + 1] / timeDiff[i] < expectedTimeScaling[i] - allowedVariability)
        {
            output = false;
        }
    }
    // Return whether each trials' times scaled correctly
    return output;
}

// Name:    Tester::removeTimeTest
// Desc:    Makes sure that the time taken for remove scales correctly with the size of the Fleet
// Precon:  inputSize should be a large positive number
//          numTrials must be > 1
//          numRepeats must be > 0
// Postcon: Returns true if the time scaling was correct between all trials
//          False otherwise
bool Tester::removeTimeTest(int inputSize, int numTrials, int numRepeats)
{
    const int inputScaling = 2;
    const double allowedVariability = .4;
    double expectedTimeScaling[numTrials - 1];
    // Fill expectedTimeScaling with the expected time scaling between trials
    for(int i = 0; i < numTrials - 1; i++)
    {
        // Summation from (j = inputSize) to (j = inputSize * inputScaling) of log2(j)
        int summationBefore = 0;
        for(int j = inputSize * pow(inputScaling, i); j <= inputSize * pow(inputScaling, i) * 2; j++)
        {
            summationBefore += log2(j);
        }
        // Summation from (j = inputSize * inputScaling) to (j = inputSize * inputScaling ^ 2) of log2(j)
        int summationAfter = 0;
        for(int j = inputSize * pow(inputScaling, i + 1); j <= inputSize * pow(inputScaling, i + 1) * 2; j++)
        {
            summationAfter += log2(j);
        }
        // Divide the summations in order to find the scaling
        expectedTimeScaling[i] = ((double) summationAfter) / summationBefore;
    }
    double timeDiff[numTrials];
    clock_t start, stop;
    // Repeat numTrials times
    for(int i = 0; i < numTrials; i++)
    {
        timeDiff[i] = 0;
        // Repeat each trial numRepeats times
        for(int j = 0; j < numRepeats; j++)
        {
            Fleet fleet;
            int ids[2 * inputSize];
            // Create and insert enough Ships for inputSize removals
            for(int k = 0; k < 2 * inputSize; k++)
            {
                do
                {
                    ids[k] = rand() % (MAXID - MINID + 1) + MINID;
                }
                while(inArray(ids[k], ids, k));
                fleet.insert(Ship(ids[k], static_cast<SHIPTYPE>(rand() % 5), ALIVE));
            }
            // Time how long it takes to remove all Ships
            start = clock();
            for(int k = 0; k < inputSize; k++)
            {
                fleet.remove(ids[k]);
            }
            stop = clock();
            timeDiff[i] += stop - start;
        }
        // Average the time between trials
        timeDiff[i] /= numRepeats;
        // Output the results of the trial to the user
        cout << "\n\tRemoving " << inputSize << " Ships from a Fleet of " << 2 * inputSize << " took an average of "
             << timeDiff[i] << " clock ticks ("<< timeDiff[i]/CLOCKS_PER_SEC << " seconds)";
        // Increase the inputSize for the next trial
        inputSize *= inputScaling;
    }
    bool output = true;
    // Compare each trial to its neighbors (ie trial 0 to 1, then trial 1 to 2...)
    for(int i = 0; i < numTrials - 1; i++)
    {
        // Output the comparison of the trials to the user
        cout << "\n\tExpected Scaling from " << inputSize / pow(inputScaling, numTrials - i) << " to " << inputSize / pow(inputScaling, numTrials - i - 1)
             << ": " << expectedTimeScaling[i] << " ± .4, Actual Scaling: " << timeDiff[i + 1] / timeDiff[i];
        // Output false if the actual timeScaling between any trials wasn't within the range expectedTimeScaling ± allowedVariability
        if(timeDiff[i + 1] / timeDiff[i] > expectedTimeScaling[i] + allowedVariability
            || timeDiff[i + 1] / timeDiff[i] < expectedTimeScaling[i] - allowedVariability)
        {
            output = false;
        }
    }
    // Return whether each trials' times scaled correctly
    return output;
}

// Name:    Tester::findShipTimeTest
// Desc:    Makes sure that the time taken for findShip scales correctly with the size of the Fleet
// Precon:  inputSize should be a large positive number
//          numTrials must be > 1
//          numRepeats must be > 0
// Postcon: Returns true if the time scaling was correct between all trials
//          False otherwise
bool Tester::findShipTimeTest(int inputSize, int numTrials, int numRepeats)
{
    const int inputScaling = 2;
    const double allowedVariability = .4;
    double expectedTimeScaling[numTrials - 1];
    // Fill expectedTimeScaling with the expected time scaling between trials
    for(int i = 0; i < numTrials - 1; i++)
    {
        // Summation from (j = 1) to (j = inputSize) of log2(j)
        int summationBefore = 0;
        for(int j = 1; j <= inputSize * pow(inputScaling, i); j++)
        {
            summationBefore += log2(j);
        }
        // Summation from (j = 1) to (j = inputSize * inputScaling) of log2(j)
        int summationAfter = summationBefore;
        for(int j = inputSize * pow(inputScaling, i) + 1; j <= inputSize * pow(inputScaling, i + 1); j++)
        {
            summationAfter += log2(j);
        }
        // Divide the summations in order to find the scaling
        expectedTimeScaling[i] = ((double) summationAfter) / summationBefore;
    }
    double timeDiff[numTrials];
    clock_t start, stop;
    // Repeat numTrials times
    for(int i = 0; i < numTrials; i++)
    {
        timeDiff[i] = 0;
        // Repeat each trial numRepeats times
        for(int j = 0; j < numRepeats; j++)
        {
            Fleet fleet;
            int ids[inputSize];
            // Create and insert enough Ships for inputSize searches
            for(int k = 0; k < inputSize; k++)
            {
                do
                {
                    ids[k] = rand() % (MAXID - MINID + 1) + MINID;
                }
                while(inArray(ids[k], ids, k));
                fleet.insert(Ship(ids[k], static_cast<SHIPTYPE>(rand() % 5), ALIVE));
            }
            // Time how long it takes to find all Ships
            start = clock();
            for(int k = 0; k < inputSize; k++)
            {
                fleet.findShip(ids[k]);
            }
            stop = clock();
            timeDiff[i] += stop - start;
        }
        // Average the time between trials
        timeDiff[i] /= numRepeats;
        // Output the results of the trial to the user
        cout << "\n\tFinding every Ship in a Fleet of " << inputSize << " took an average of "
             << timeDiff[i] << " clock ticks ("<< timeDiff[i]/CLOCKS_PER_SEC << " seconds)";
        // Increase the inputSize for the next trial
        inputSize *= inputScaling;
    }
    bool output = true;
    // Compare each trial to its neighbors (ie trial 0 to 1, then trial 1 to 2...)
    for(int i = 0; i < numTrials - 1; i++)
    {
        // Output the comparison of the trials to the user
        cout << "\n\tExpected Scaling from " << inputSize / pow(inputScaling, numTrials - i) << " to " << inputSize / pow(inputScaling, numTrials - i - 1)
             << ": " << expectedTimeScaling[i] << " ± .4, Actual Scaling: " << timeDiff[i + 1] / timeDiff[i];
        // Output false if the actual timeScaling between any trials wasn't within the range expectedTimeScaling ± allowedVariability
        if(timeDiff[i + 1] / timeDiff[i] > expectedTimeScaling[i] + allowedVariability
            || timeDiff[i + 1] / timeDiff[i] < expectedTimeScaling[i] - allowedVariability)
        {
            output = false;
        }
    }
    // Return whether each trials' times scaled correctly
    return output;
}

// Name:    Tester::inArray
// Desc:    Checks whether the passed int is in the array of ints
// Precon:  size denotes the size of the passed array
// Postcon: If the int is found in the array, returns true
//          Else returns false
bool Tester::inArray(int item, int arr[], int size)
{
    // Iterate through the passed array
    for(int i = 0; i < size; i++)
    {
        // The passed int was found, return true
        if(arr[i] == item)
        {
            return true;
        }
    }
    // Never found, return false
    return false;
}

// Name:    Tester::copyFleet
// Desc:    Copy constructor for a Fleet
// Precon:  None
// Postcon: Returns a deep copy of the passed Fleet
Fleet Tester::copyFleet(const Fleet& fleet)
{
    Fleet copy;
    copy.m_root = copyShip(fleet.m_root);
    return copy;
}

// Name:    Tester::copyShip
// Desc:    Recursively copies each Ship in the passed subtree
// Precon:  None
// Postcon: Returns a deep copy of the passed subtree
Ship* Tester::copyShip(Ship* ship)
{
    // Base case, no Ship to copy
    if(ship == nullptr)
    {
        return nullptr;
    }
    // Copy the Ship and its subtrees
    else
    {
        Ship* copy = new Ship(ship->m_id, ship->m_type, ship->m_state);
        copy->m_color = ship->m_color;
        copy->m_left = copyShip(ship->m_left);
        copy->m_right = copyShip(ship->m_right);
        return copy;
    }
}

// Name:    Tester::unbalanced
// Desc:    Checks if a passed Fleet is a BST and a Red-Black Tree
// Precon:  None
// Postcon: If the Fleet is a valid Red-Black Tree, returns false
//          Else returns true
bool Tester::unbalanced(const Fleet& fleet)
{
    return recursBalanced(fleet.m_root) < 0;
}

// Name:    Tester::recursBalanced
// Desc:    Recursively checks if each subtree is a valid Red-Black subtree
// Precon:  None
// Postcon: If finding imbalance, returns -1
//          Else returns the number of BLACK Ships in path to null
int Tester::recursBalanced(Ship* ship)
{
    // Base case, null leaf found, return 0
    if(ship == nullptr)
    {
        return 0;
    }
    // Check that the subtree is balanced
    else
    {
        // Check that each of the Ship's subtrees are balanced
        int left = recursBalanced(ship->m_left), right = recursBalanced(ship->m_right);
        // An unbalance is found, return -1
        if(left < 0                             // If there is an unbalance on the left
            || right < 0                        // or an unbalance on the right
            || left != right                    // or the count of BLACK Ships on the null paths differ between left and right
            || (ship->m_left != nullptr         // or the Ship has a left child
            && (ship->m_left->m_id >= ship->m_id    // and the child is greater or equal to the Ship
            || (ship->m_left->m_color == RED        // or the child is RED
            && ship->m_color == RED)))                  // and the Ship is RED
            || (ship->m_right != nullptr        // or the Ship has a right child
            && (ship->m_right->m_id <= ship->m_id   // and the child is less or equal to the Ship
            || (ship->m_right->m_color == RED       // or the child is RED
            && ship->m_color == RED)))                  // and the Ship is RED
            || ship->m_color == DOUBLEBLACK)    // or the Ship is DOUBLEBLACK
        {
            return -1;
        }
        // No unbalance, return the count of BLACK Ships on the path to null
        else
        {
            // Iterate the count of BLACK Ships on the null path
            if(ship->m_color == BLACK)
            {
                left++;
            }
            return left;
        }
    }
}

// Name:    Tester::fleetEqual
// Desc:    Checks that the passed Fleets contain the same data
// Precon:  None
// Postcon: If the passed Fleets contain the same data, returns true
//          Else returns false
bool Tester::fleetEqual(const Fleet& lhs, const Fleet& rhs)
{
    return shipEqual(lhs.m_root, rhs.m_root);
}

// Name:    Tester::shipEqual
// Desc:    Checks the passed Ship and its subtrees contain the same data
// Precon:  None
// Postcon: If the passed Ships and subtrees contain the same data, returns true
//          Else returns false
bool Tester::shipEqual(Ship* lhs, Ship* rhs)
{
    // Nullptrs are equal, return true
    if(lhs == nullptr
        && rhs == nullptr)
    {
        return true;
    }
    // There is only one nullptr, return false
    else if(lhs == nullptr
        || rhs == nullptr)
    {
        return false;
    }
    // Data is the same in both Ships and their subtrees, return true
    else if(shipEqual(lhs->m_left, rhs->m_left)
        && shipEqual(rhs->m_right, rhs->m_right)
        && lhs->m_id == rhs->m_id
        && lhs->m_type == rhs->m_type
        && lhs->m_state == rhs->m_state
        && lhs->m_color == rhs->m_color)
    {
        return true;
    }
    // Different data was found, return false
    else
    {
        return false;
    }
}

int main()
{
    Tester test;
    srand(time(NULL));

    // Creating a standard Fleet to be used in tests
    Fleet normal;
    const int normalSize = 500;
    int normalIds[normalSize];
    for(int i = 0; i < normalSize; i++)
    {
        do
        {
            normalIds[i] = rand() % (MAXID - MINID + 1) + MINID;
        }
        while(Tester::inArray(normalIds[i], normalIds, i));
        normal.insert(Ship(normalIds[i], static_cast<SHIPTYPE>(rand() % 5), ALIVE));
    }

    cout << BREAK << "Testing insert(Ship&)\n" << BREAK << endl;
    {   cout << "Normal: Inserting " << normalSize << " Ships into an empty Fleet (This includes edge cases like inserting at the root)";
        Fleet copy;
        int ids[normalSize];
        Ship ships[normalSize];
        for(int i = 0; i < normalSize; i++)
        {
            do
            {
                ids[i] = rand() % (MAXID - MINID + 1) + MINID;
            }
            while(Tester::inArray(ids[i], ids, i));
            ships[i] = Ship(ids[i], static_cast<SHIPTYPE>(rand() % 5), ALIVE);
        }
        test.result(Tester::insertTest(copy, ships, normalSize));
    }
    {   cout << "Edge: Inserting Ships whose ids are MINID and MAXID";
        Fleet copy = Tester::copyFleet(normal);
        Ship ships[2] = {Ship(MINID, static_cast<SHIPTYPE>(rand() % 5), ALIVE), Ship(MAXID, static_cast<SHIPTYPE>(rand() % 5), ALIVE)};
        test.result(Tester::insertTest(copy, ships, 2));
    }
    {   cout << "Edge: Inserting a BLACK Ship";
        Fleet copy = Tester::copyFleet(normal);
        Ship ships[1] = {Ship(rand() % (MAXID - MINID + 1) + MINID, static_cast<SHIPTYPE>(rand() % 5), ALIVE)};
        ships[1].setColor(BLACK);
        test.result(Tester::insertTest(copy, ships, 1));
    }
    {   cout << "Error: Inserting Ships with already existing ids";
        Fleet copy = Tester::copyFleet(normal);
        Ship ships[normalSize];
        for(int i = 0; i < normalSize; i++)
        {
            ships[i] = Ship(normalIds[i], static_cast<SHIPTYPE>(rand() % 5), ALIVE);
        }
        test.result(Tester::insertTest(copy, ships, normalSize));
    }
    {   cout << "Error: Inserting Ships whose ids are below MINID and above MAXID";
        Fleet copy = Tester::copyFleet(normal);
        Ship ships[2] = {Ship(MINID - 1, static_cast<SHIPTYPE>(rand() % 5), ALIVE), Ship(MAXID + 1, static_cast<SHIPTYPE>(rand() % 5), ALIVE)};
        test.result(Tester::insertTest(copy, ships, 2));
    }
    {   cout << "Testing time complexity";
        test.result(Tester::insertTimeTest());
    }

    cout << BREAK << "Testing remove(int)\n" << BREAK << endl;
    {   cout << "Normal: Removing all Ships from a Fleet of " << normalSize << " (This includes edge cases like removing the root)";
        Fleet copy = Tester::copyFleet(normal);
        test.result(Tester::removeTest(copy, normalIds, normalSize));
    }
    {   cout << "Error: Removing from an empty Fleet";
        Fleet copy;
        int ids[1] = {rand() % (MAXID - MINID + 1) + MINID};
        test.result(Tester::removeTest(copy, ids, 1));
    }
    {   cout << "Error: Removing a nonexisting id";
        Fleet copy = Tester::copyFleet(normal);
        int ids[1];
        do
        {
            ids[0] = rand() % (MAXID - MINID + 1) + MINID;
        }
        while(Tester::inArray(ids[0], normalIds, normalSize));
        test.result(Tester::removeTest(copy, ids, 1));
    }
    {   cout << "Error: Removing ids below MINID and above MAXID";
        Fleet copy = Tester::copyFleet(normal);
        int ids[2] = {MINID - 1, MAXID + 1};
        test.result(Tester::removeTest(copy, ids, 2));
    }
    {   cout << "Testing time complexity:";
        test.result(Tester::removeTimeTest());
    }

    cout << BREAK << "Testing setState(int, STATE)\n" << BREAK << endl;
    {   cout << "Normal: Losing a Ship in a Fleet of " << normalSize;
        Fleet copy = Tester::copyFleet(normal);
        test.result(Tester::setStateTest(copy, normalIds[rand() % normalSize]));
    }
    {   cout << "Error: Losing a Ship in an empty Fleet";
        Fleet copy;
        test.result(Tester::setStateTest(copy, rand() % (MAXID - MINID + 1) + MINID));
    }
    {   cout << "Error: Losing a Ship with a nonexisting id";
        Fleet copy = Tester::copyFleet(normal);
        int id;
        do
        {
            id = rand() % (MAXID - MINID + 1) + MINID;
        }
        while(Tester::inArray(id, normalIds, normalSize));
        test.result(Tester::setStateTest(copy, id));
    }
    {   cout << "Error: Losing Ships with ids below MINID and above MAXID";
        Fleet copy = Tester::copyFleet(normal);
        test.result(Tester::setStateTest(copy, MINID - 1) && Tester::setStateTest(copy, MAXID + 1));
    }

    cout << BREAK << "Testing removeLost()\n" << BREAK << endl;
    {   cout << "Normal: " << normalSize / 2 << " lost Ships in a Fleet of " << normalSize;
        Fleet copy = Tester::copyFleet(normal);
        for(int i = 0; i < normalSize / 2; i++)
        {
            copy.setState(normalIds[i], LOST);
        }
        test.result(Tester::removeLostTest(copy, normalIds, normalSize / 2));
    }
    {   cout << "Edge: All Ships lost in a Fleet of " << normalSize;
        Fleet copy = Tester::copyFleet(normal);
        for(int i = 0; i < normalSize; i++)
        {
            copy.setState(normalIds[i], LOST);
        }
        test.result(Tester::removeLostTest(copy, normalIds, normalSize));
    }
    {   cout << "Edge: 0 lost Ships";
        Fleet copy = Tester::copyFleet(normal);
        test.result(Tester::removeLostTest(copy, normalIds, 0));
    }
    {   cout << "Edge: empty Fleet";
        Fleet copy;
        test.result(Tester::removeLostTest(copy, {}, 0));
    }

    cout << BREAK << "Testing findShip(int)\n" << BREAK << endl;
    {   cout << "Normal: Finding all Ships in a Fleet of " << normalSize;
        Fleet copy = Tester::copyFleet(normal);
        test.result(Tester::findShipTest(copy, normalIds, normalSize, true));
    }
    {   cout << "Normal: Finding a Ship with a nonexisting id";
        Fleet copy = Tester::copyFleet(normal);
        int ids[1];
        do
        {
            ids[0] = rand() % (MAXID - MINID + 1) + MINID;
        }
        while(Tester::inArray(ids[0], normalIds, normalSize));
        test.result(Tester::findShipTest(copy, ids, 1, false));
    }
    {   cout << "Edge: Finding a Ship in an empty Fleet";
        Fleet copy;
        int ids[1] = {rand() % (MAXID - MINID + 1) + MINID};
        test.result(Tester::findShipTest(copy, ids, 1, false));
    }
    {   cout << "Edge: Finding Ships whose ids are below MINID and above MAXID";
        Fleet copy = Tester::copyFleet(normal);
        int ids[2] = {MINID - 1, MAXID + 1};
        test.result(Tester::findShipTest(copy, ids, 2, false));
    }
    {   cout << "Testing time complexity:";
        test.result(Tester::findShipTimeTest());
    }

    cout << BREAK << "Number of tests: " << test.getTestCount()
         << "\nNumber of tests failed: " << test.getFailCount()
         << endl << BREAK;
}