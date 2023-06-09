#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "courseNode.h"
#include <sstream>

using namespace std;

unordered_map<string, courseNode> ReadCourses(string filename)
{
    unordered_map<string, courseNode> course_id;
    ifstream infile(filename);
    string line;
    while (getline(infile, line))
    {
        if (line != "")
        {
            stringstream ss(line);
            string id, word;
            ss >> id;
            bool isNew = true;
            auto foundObj = course_id.find(id); // checking if the course exist in the map
            courseNode obj;
            if (foundObj != course_id.end())
            { // course already exist
                obj = foundObj->second;
                isNew = false;
            }
            else
            { // new course
                obj = courseNode(id);
            }
            vector<string> prereqvector;

            while (ss >> word)
            {
                if (word == id)
                {
                    continue;
                }
                bool isAlreadyPrereq = false;
                for (auto const prereq : obj.prereq) // looping through the vector
                {
                    for (auto const prereqCourseID : prereq) // looping through each index of the vector . i.e vector of vector
                        if (prereqCourseID == word)
                        {
                            isAlreadyPrereq = true;
                            break;
                        }
                }
                if (isAlreadyPrereq)
                {
                    continue; // skip adding already present prerequisite
                }
                string prereq_ID;
                courseNode prereq_obj(word);
                auto foundPrereq = course_id.find(word);
                if (foundPrereq != course_id.end())
                { // pre req already exist in map
                    prereq_ID = (foundPrereq->second.courseName);
                }
                else
                { // pre req does not exist in map

                    course_id.emplace(word, prereq_obj);
                    prereq_ID = word;
                }
                prereqvector.push_back(prereq_ID);
            }
            obj.addPrereq(prereqvector);

            if (isNew)
            {
                course_id.emplace(id, obj);
            }
            else
            {
                course_id[id] = obj;
            }
        }
    }
    infile.close();
    return course_id;
}

bool dfs(courseNode node, unordered_map<string, courseNode> courses, vector<string> visited, vector<string> &depend)
{
    //Mark the current node as visited
    visited.push_back(node.courseName);

    //Go through all the prerequisites of the current node
    for (auto const prereqVector : node.prereq)
    {
        for (auto const prereqCourse : prereqVector)
        {
            depend.push_back(node.courseName);
            //Visit the prerequisite course
            if (find(visited.begin(), visited.end(),prereqCourse) == visited.end())
            {   
                //recursively call the dfs, go through the prere
                if (dfs(courses[prereqCourse], courses, visited,depend ))
                {
                    //circular dependency found
                    return true; 
                }
            }
            // If the prerequisite has already been visited and is still in the call stack, there is a circular dependency
            else if (find(visited.begin(), visited.end(),prereqCourse ) != visited.end())
            {
                return true; // circular dependency found, return true
            }
        }
    }
    //Remove the current node from the call stack
    visited.pop_back();

    //If no circular dependencies were found, return false
    return false;
}

int main(int argc, char *argv[])
{
    string filename = argv[1];
    unordered_map<string, courseNode> course_id = ReadCourses(filename);
    cout << "\n----------------\n";
    
    vector<string> visited;
    vector<string> depend; 
    //use to check if the course is Viable
    bool isViable = true;

    for (auto const [id, node] : course_id)
    {
        visited.clear();
        if (dfs(node, course_id, visited, depend))
        {
            cout << "Not Viable: prerequisites have circular dependencies" << endl;
            
            //print the dependency out
            cout << "Dependency : " ; 
            for(int i = 0 ; i < depend.size() ; i ++)
            {
                cout << depend[i] << " " ;
            }

            cout << endl;
            isViable = false;
            break;
        }
    }

    if (isViable)
    {
         cout << "Viable!!!" << endl << endl;

        int prerequisitesCount = 0;
        for (auto const &[id, node] : course_id)
        {
            cout << node.courseName << " Prereq: ";
            for (auto const &prereqVector : node.prereq)
            {
                cout << "[";
                for (auto const &prereqID : prereqVector)
                {
                    cout << " " << prereqID;
                }
                cout << " ] ";
            }

            if (node.prereq.size() > 6)
            {
                cout << "\nNot Viable: Exceeds 6 prerequisites";
            }

            cout << endl;
        }
    }
}