#ifndef __MABE__Parameters__
#define __MABE__Parameters__

#include "Utilities.h"
#include "AssertWithMessage.h"
#include <type_traits>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include <memory>
#include <vector>

using namespace std;

//template<class T1, class T2, class Pred = std::less<T2> >
//struct sort_pair_first {
//	bool operator()(const std::pair<T1, T2>&left, const std::pair<T1, T2>&right) {
//		Pred p;
//		return p(left.first, right.first);
//	}
//};
//
//template<class T1, class T2, class Pred = std::less<T2> >
//struct sort_pair_second {
//	bool operator()(const std::pair<T1, T2>&left, const std::pair<T1, T2>&right) {
//		Pred p;
//		return p(left.second, right.second);
//	}
//};

//// this is here so we can use to string and it will work even if we give it a string as input
//inline string to_string(string val) {
//	return val;
//}
//
//template<typename T1, typename T2>
//bool HaveSameType(T1, T2) {
//	return is_same<T1, T2>();
//}
//
//inline string get_var_typename(const bool&) {
//	return "bool";
//}
//inline string get_var_typename(const string&) {
//	return "string";
//}
//inline string get_var_typename(const int&) {
//	return "int";
//}
//inline string get_var_typename(const double&) {
//	return "double";
//}

//inline vector<string> nameSpaceToNameParts(const string& nameSpace) {
//	string localNameSpace = nameSpace;
//	vector<string> nameParts;
//	bool done = (nameSpace.size() == 0);
//	bool nameSpaceValid = true;
//	while (!done) {
//		if (localNameSpace.size() > 2) {  // name space name must end with :: so must have more then 2 characters
//			int i = 0;
//			while ((localNameSpace[i] != ':' || localNameSpace[i + 1] != ':') && (i < (int) localNameSpace.size() - 2)) {
//				i++;
//			}
//			if (!(localNameSpace[i] == ':' && localNameSpace[i + 1] == ':')) {  // if there is not "::" at the end of the name space part
//				nameSpaceValid = false;
//				done = true;
//			} else {  // found "::"
//				nameParts.push_back(localNameSpace.substr(0, i + 2));
//				localNameSpace = localNameSpace.substr(i + 2, localNameSpace.size());
//				if (localNameSpace.size() == 0) {  // if after cutting of part there is nothing left
//					done = true;
//				}
//			}
//		} else {  // if the name space name part is less then 3 characters... ie, must be at least "*::"
//			nameSpaceValid = false;
//			done = true;
//		}
//	}
//	if (!nameSpaceValid) {
//		cout << "  Error::in ParametersTable::pointToNestedTable(string nameSpace). name space is invalid.\n  Can't parse \"" << localNameSpace << "\"\n   Parameter name space must end in ::\nExiting." << endl;
//		exit(1);
//	}
//	return nameParts;
//}

//// Put an arbitrary value to the target variable, return false on conversion failure
//template<class T>
//static bool stringToValue(const string& source, T& target) {
//	std::stringstream ss(source);
//	ss >> target;
//	if (ss.fail()) {
//		return false;
//	} else {
//		string remaining;
//		ss >> remaining;
//		// stream failure means nothing left in stream, which is what we want
//		return ss.fail();
//	}
//}

class AbstractParametersEntry {
protected:
	bool local = 0;
	bool saveOnFileWrite = false;

public:

	inline void setSaveOnFileWrite(bool _saveOnFileWrite = true) {
		saveOnFileWrite = _saveOnFileWrite;
	}

	inline bool saveOnWrite() {
		return saveOnFileWrite;
	}

	virtual inline bool isLocal() {
		return local;
	}

	virtual ~AbstractParametersEntry() = default;

	virtual const bool getBool() {
		ASSERT(false, "using abstract class method");
		return (false);
	}
	virtual const string getString() {
		ASSERT(false, "using abstract class method");
		return ("");
	}
	virtual const int getInt() {
		ASSERT(false, "using abstract class method");
		return (0);
	}
	virtual const double getDouble() {
		ASSERT(false, "using abstract class method");
		return (0);
	}

	virtual const void get(bool& value) {
		ASSERT(false, "using abstract class method for get(bool) - suggests a type mismatch in parameter types.");
	}
	virtual const void get(string& value) {
		ASSERT(false, "using abstract class method for get(string) - suggests a type mismatch in parameter types.");
	}
	virtual const void get(int& value) {
		ASSERT(false, "using abstract class method for get(int) - suggests a type mismatch in parameter types.");
	}
	virtual const void get(double& value) {
		ASSERT(false, "using abstract class method for get(double) - suggests a type mismatch in parameter types.");
	}

	virtual void set(const bool & val, bool _local = true) {
		ASSERT(false, "using abstract class method for set(bool) - suggests a type mismatch in parameter types.");
	}
	virtual void set(const string & val, bool _local = true) {
		ASSERT(false, "using abstract class method for set(string) - suggests a type mismatch in parameter types.");
	}
	virtual void set(const int & val, bool _local = true) {
		ASSERT(false, "using abstract class method for set(int) - suggests a type mismatch in parameter types.");
	}
	virtual void set(const double & val, bool _local = true) {
		ASSERT(false, "using abstract class method for set(double) - suggests a type mismatch in parameter types.");
	}

	virtual void getValuePtr(shared_ptr<bool> &ptr) {
		ASSERT(false, "using abstract class method");
	}
	virtual void getValuePtr(shared_ptr<string> &ptr) {
		ASSERT(false, "using abstract class method");
	}
	virtual void getValuePtr(shared_ptr<int> &ptr) {
		ASSERT(false, "using abstract class method");
	}
	virtual void getValuePtr(shared_ptr<double> &ptr) {
		ASSERT(false, "using abstract class method");
	}
	virtual void follow(shared_ptr<AbstractParametersEntry> &ptr) {
		ASSERT(false, "using abstract class method");
	}

	virtual string getTypeName() {
		ASSERT(false, "using abstract class method");
		return "";
	}

	virtual pair<string, string> getStringValuePair() {
		ASSERT(false, "using abstract class method");
		return {"",""};
	}

	virtual void show() {
		ASSERT(false, "using abstract class method");
	}
};

template<typename T>
class ParametersEntry: public AbstractParametersEntry {
private:
	shared_ptr<T> valuePtr;

public:
	ParametersEntry() = default;

	ParametersEntry(T value, bool _local = true) {
		set(value, _local);
	}

	const T & get() {
		return *valuePtr;
	}

	const void get(T& value) override {
		value = *valuePtr;
	}

	const bool getBool() override {
		ASSERT(0, ("in ParametersEntry::GetBool() value associated with this ParamatersEntry is not bool"));
		exit(1);
	}

	const string getString() override {
		ASSERT(0, ("in ParametersEntry::GetString() value associated with this ParamatersEntry is not string"));
		exit(1);
	}

	const int getInt() override {
		ASSERT(0, ("in ParametersEntry::GetInt() value associated with this ParamatersEntry is not int"));
		exit(1);
	}

	const double getDouble() override {
		ASSERT(0, ("in ParametersEntry::GetDouble() value associated with this ParamatersEntry is not double"));
		exit(1);
	}

	void set(const T & value, bool _local = true) override {
		if (valuePtr == nullptr) {
			valuePtr = make_shared<T>(value);
		} else {
			*valuePtr = value;
		}
		local = _local;
	}

	void getValuePtr(shared_ptr<T> &ptr) override {
		ASSERT(valuePtr != nullptr, "in ParametersEntry(shared_ptr<T> &ptr) :: Attempt to get shared pointer from broken pointer!");
		ptr = valuePtr;
	}

	void follow(shared_ptr<AbstractParametersEntry> &ptr) override {
		ASSERT(ptr != nullptr, "in follow(shared_ptr<AbstractParametersEntry> &ptr) :: ptr = nullptr");
		shared_ptr<ParametersEntry<T>> castParametersEntry = dynamic_pointer_cast<ParametersEntry<T>>(ptr);
		ASSERT(castParametersEntry->valuePtr != nullptr, "in follow(shared_ptr<AbstractParametersEntry> &ptr) :: value in ptr = nullptr");
		valuePtr = castParametersEntry->valuePtr;
		local = false;
	}

	virtual string getTypeName() {
		return get_var_typename(*valuePtr);
	}

	// return a pair of string with <value,type>
	pair<string, string> getStringValuePair() override {
		ostringstream value;
		string trimmedValue = "";
		value << fixed << *valuePtr;
		if (getTypeName() == "double") {
			bool foundDot = false;
			bool foundNonZero = false;

			int i = (int) value.str().size() - 1;
			while (i >= 0) {
				if (value.str()[i] != '0') {
					foundNonZero = true;
				}
				if (value.str()[i] == '.') {
					foundDot = true;
					if (trimmedValue == "") {
						trimmedValue = "0";
					}
				}
				if (value.str()[i] != '0' || foundNonZero) {
					trimmedValue = value.str()[i] + trimmedValue;
				}
				i--;
			}
			if (!foundDot) {
				trimmedValue = value.str();
			}
			return {trimmedValue,getTypeName()};
		}
		return {value.str(),getTypeName()};
	}

	void show() {
		cout << "\n        Value = ";
		cout << *valuePtr << " (" << get_var_typename(*valuePtr) << ")";
		if (local) {
			cout << " is Local";
		} else {
			cout << " is not Local";
		}
		cout << endl;
		cout << "        saveOnFileWrite: ";
		if (saveOnFileWrite) {
			cout << "yes" << endl;
		} else {
			cout << "no" << endl;

		}
	}

};

class ParametersTable {
private:
	static long long nextTableID;
	long long ID;
	string tableNameSpace;
	shared_ptr<ParametersTable> rootTable;
	shared_ptr<ParametersTable> parent;
	unordered_map<long long, shared_ptr<ParametersTable>> children;
	unordered_map<string, shared_ptr<AbstractParametersEntry>> table;
	shared_ptr<map<string, shared_ptr<ParametersTable>>> parametersTablesRegistry;
	shared_ptr<map<string, string>> parameterDocumentation;  // for each parameter name, contains documentation

	string workString;

	void setID() {
		ID = nextTableID++;
	}

	void clearParent() {
		if (parent != nullptr) {  // if there is a parent, make sure to remove this table from that parents children so that this will be deleted.
			parent->children.erase(ID);
			parent.reset();
		}
	}

	void addChild(shared_ptr<ParametersTable> _child) {
		children[_child->ID] = _child;
	}

public:
	bool neverSave = 0;

	ParametersTable() = delete;

	ParametersTable(const string& _tableNameSpace = "", shared_ptr<ParametersTable> _rootTable = nullptr) :
	tableNameSpace(_tableNameSpace), rootTable(_rootTable) {
		setID();
	}

	~ParametersTable() = default;

	long long getID() {
		return ID;
	}

	string getTableNameSpace() {
		return tableNameSpace;
	}

	string getParameterType(const string& name) {
		if(rootTable->table.find(name)==table.end()) {
			return("FAIL");
		} else {
			return (rootTable->table[name]->getTypeName());
		}
	}

	static shared_ptr<ParametersTable> addTableWithParent(const string& _tableNameSpace, shared_ptr<ParametersTable> _rootTable, shared_ptr<ParametersTable> _parent) {
		shared_ptr<ParametersTable> newTable = make_shared<ParametersTable>(_tableNameSpace, _rootTable);
		if (_parent == nullptr) {  //this will be a root table
			newTable->parametersTablesRegistry = make_shared<map<string, shared_ptr<ParametersTable>>>();
			newTable->parameterDocumentation = make_shared<map<string, string>>();// for each parameter name, contains documentation
		} else {
			newTable->parent = _parent;
			newTable->parent->addChild(newTable);
			newTable->parametersTablesRegistry = _parent->parametersTablesRegistry;
			newTable->parameterDocumentation = _parent->parameterDocumentation;
		}
		(*newTable->parametersTablesRegistry)[_tableNameSpace]=newTable;
		return newTable;
	}

	static shared_ptr<ParametersTable> makeTable(const string& _tableNameSpace = "", shared_ptr<ParametersTable> _rootTable = nullptr) {
		if (_rootTable == nullptr) {
			shared_ptr<ParametersTable> newTable = addTableWithParent(_tableNameSpace, _rootTable, nullptr);
			newTable->rootTable = newTable;
			return newTable;
		} else if ((*_rootTable->parametersTablesRegistry).find(_tableNameSpace) != (*_rootTable->parametersTablesRegistry).end()) {
			cout << "  Warning :: call to makeTable. table with nameSpace: \"" << _tableNameSpace << "\" already exists. Ignoring..."<< endl;
			return (*_rootTable->parametersTablesRegistry)[_tableNameSpace];
		} else {
			shared_ptr<ParametersTable> searchTable = nullptr;
			vector<string> nameSpaceParts = nameSpaceToNameParts(_tableNameSpace);
			int index = 0;
			bool looking = true;
			string nameSpace = "";
			while (index < (int) nameSpaceParts.size() && looking) {  // as long as we keep finding tables in the registry, keep checking down
				nameSpace += nameSpaceParts[index];
				auto s = (*_rootTable->parametersTablesRegistry).find(nameSpace);
				if (s != (*_rootTable->parametersTablesRegistry).end()) {
					searchTable = s->second;  // found a table, keep looking to see if there is one farther down
				} else {
					looking = false;  // we did not find a table! searchTable is the last table we found (or nullptr) and paramaterName is in a table we must make
				}
				index++;
			}
			if (searchTable == nullptr) {
				searchTable = _rootTable;
			}
			shared_ptr<ParametersTable> newTable = addTableWithParent(nameSpace, _rootTable, searchTable);
			searchTable = newTable;
			while (index < (int) nameSpaceParts.size()) {
				nameSpace += nameSpaceParts[index];
				newTable = addTableWithParent(nameSpace, _rootTable, searchTable);
				searchTable = newTable;
				index++;
			}
			return searchTable;
		}
	}

	// looks for a table with name space _tableNameSpace. if found, return shared ptr to table, if not, create table and return shared ptr
	shared_ptr<ParametersTable> getTable(const string& _tableNameSpace) {
		if ((*rootTable->parametersTablesRegistry).find(_tableNameSpace) != (*rootTable->parametersTablesRegistry).end()) {
			return (*rootTable->parametersTablesRegistry)[_tableNameSpace];
		} else {
			return makeTable(_tableNameSpace, rootTable);
		}
	}

	// looks for a table with name space _tableNameSpace; if not found error out.
	shared_ptr<ParametersTable> lookupTable(const string& _tableNameSpace) {
		ASSERT ((*parametersTablesRegistry).find(_tableNameSpace) != (*parametersTablesRegistry).end() , ("ERROR! :: Attempt to lookup nonexistent table with name \"" + _tableNameSpace + "\". Exiting!\n"));
		return (*parametersTablesRegistry)[_tableNameSpace];
	}

	// finds table where name exists, starting at searchTable and working up to root. If not found, terminate.
	shared_ptr<ParametersTable> findTableWithNamedParameter(const string& name, shared_ptr<ParametersTable> searchTable) {
		bool found = false;
		while (searchTable != nullptr && !found) {  // while findTable is valid and we have not found the value
			if (searchTable->table.find(name) == table.end()) {  // if the value is not here, point to the parent.
				searchTable = searchTable->parent;
			} else if (!searchTable->table[name]->isLocal()) {  // if the value is here, but is not local, point to the parent.
				searchTable = searchTable->parent;
			} else {  // if the value is here and is local, then set found = true
				found = true;
			}
		}
		return searchTable;
		// searchTable either points to a valid table which contains a local value for the named parameter or is a nullptr if not found
	}

	//return a shared_ptr to Entry in this table with name. If not found, search for it higher.
	shared_ptr<ParametersEntry<bool>> lookupBoolEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupBool(name);
		}
		return dynamic_pointer_cast<ParametersEntry<bool>>(table[name]);
	}

	shared_ptr<ParametersEntry<string>> lookupStringEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupString(name);
		}
		return dynamic_pointer_cast<ParametersEntry<string>>(table[name]);
	}

	shared_ptr<ParametersEntry<int>> lookupIntEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupInt(name);
		}
		return dynamic_pointer_cast<ParametersEntry<int>>(table[name]);
	}

	shared_ptr<ParametersEntry<double>> lookupDoubleEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupDouble(name);
		}
		return dynamic_pointer_cast<ParametersEntry<double>>(table[name]);
	}

	template<typename T>
	void lookup(const string& name, T& value) {
		// check for the name in this table
		if (table.find(name) != table.end()) {  // if this table has entry called name
			table[name]->get(value);
		} else {
			//value = 10.0;
			shared_ptr<ParametersTable> searchTable;// used to determine which ancestor table we are looking at
			if (parent != nullptr) {
				searchTable = parent;
			} else {
				searchTable = rootTable;
			}
			searchTable = findTableWithNamedParameter(name, searchTable);  // find the name in the closest ancestor table with named parameter
			ASSERT (searchTable != nullptr,"  ERROR! :: in ParametersTable::lookup(const string& name, bool& value) - could not find \"" << tableNameSpace << name << "\" in parameters tables! Exiting!");
			searchTable->table[name]->get(value);// assign value
			////table[name] = make_shared<ParametersEntry<T>>(value,false);// create new entry with value and local = false
			table[name] = make_shared<ParametersEntry<T>>();
			table[name]->follow(searchTable->table[name]);
		}
	}

	inline bool lookupBool(const string& name) {
		bool value;
		lookup(name, value);
		return value;
	}
	inline string lookupString(const string& name) {
		string value;
		lookup(name, value);
		return value;
	}
	inline int lookupInt(const string& name) {
		int value;
		lookup(name, value);
		return value;
	}
	inline double lookupDouble(const string& name) {
		double value;
		lookup(name, value);
		return value;
	}

	inline bool lookupBool(const string& name, const string& _tableNameSpace) {
		ASSERT(parametersTablesRegistry->find(_tableNameSpace)!=parametersTablesRegistry->end(),"  ERROR! :: in ParametersTable::lookupBool(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!");
		bool value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}
	inline string lookupString(const string& name, const string& _tableNameSpace) {
		ASSERT(parametersTablesRegistry->find(_tableNameSpace)!=parametersTablesRegistry->end(),"  ERROR! :: in ParametersTable::lookupString(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!");
		string value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}
	inline int lookupInt(const string& name, const string& _tableNameSpace) {
		ASSERT(parametersTablesRegistry->find(_tableNameSpace)!=parametersTablesRegistry->end(),"  ERROR! :: in ParametersTable::lookupInt(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!");
		int value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}
	inline double lookupDouble(const string& name, const string& _tableNameSpace) {
		ASSERT(parametersTablesRegistry->find(_tableNameSpace)!=parametersTablesRegistry->end(),"  ERROR! :: in ParametersTable::lookupDouble(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!");
		double value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}

	// set value in named table (creates if needed) - will also set this value in rootTable if the value does not exist
	// add or overwrite to this table and add to root if not there.
	template<typename T>
	void setParameter(const string& name, const T& value, const string& _tableNameSpace = "'", bool _saveOnFileWrite = false) {
		//shared_ptr<AbstractParametersEntry> setParameter(const string& name, const T& value, const string& _tableNameSpace = "'", bool _saveOnFileWrite = false) {
		string localTableNameSpace = (_tableNameSpace == "'")?tableNameSpace:_tableNameSpace;
		if (localTableNameSpace != tableNameSpace) {  // if this table is not the table we are writing to...nameSpaceToNameParts
			if ((*parametersTablesRegistry).find(localTableNameSpace) != (*parametersTablesRegistry).end()) {  // if the table we are writing to exists...
				return (*parametersTablesRegistry)[localTableNameSpace]->setParameter(name, value, localTableNameSpace, _saveOnFileWrite);// go to the table and set the value
			} else {  // if the table we are writting to does not exist...
				return makeTable(localTableNameSpace, rootTable)->setParameter(name, value, localTableNameSpace, _saveOnFileWrite);// make the table and set value in table
			}
		} else {  // if this is the table we are writing to...
			if (table.find(name) != table.end()) {  // if this table has entry called name
				table[name]->set(value);
			} else {  // this table does not have entry called name
				table[name] = make_shared<ParametersEntry<T>>(value);
				if (parent != nullptr) {  // if this is not a root table...
					if (rootTable->table.find(name) == rootTable->table.end()) {  // if this value is not yet set in the root table. create it there also
						rootTable->table[name] = make_shared<ParametersEntry<T>>(value);
					}
				}
			}
			vector<ParametersTable> checklist;  // make a checklist of all children tables from the current table
			for (auto c : children) {
				checklist.push_back(*c.second);
			}
			while (checklist.size() > 0) {
				ParametersTable checkTable = checklist[checklist.size() - 1];  // get last element in checklist
				checklist.pop_back();
				if (checkTable.table.find(name) != checkTable.table.end()) {  // if this table has entry called name
					if (!checkTable.table[name]->isLocal()) {  // if it's not local, then we need to change where it's looking
						//checkTable.table[name]->set(name,false);
						checkTable.table[name]->follow(table[name]);
						for (auto c : checkTable.children) {  // and add it's children to checklist
							checklist.push_back(*c.second);
						}
					}  // if isLocal we don't do anything! (if children are referencing, they are referencing here)
				} else {  // if name is not here, add children to checklist (a child may be referencing above.)
					for (auto c : checkTable.children) {  // and add it's children to checklist
						checklist.push_back(*c.second);
					}
				}

			}
			if (_saveOnFileWrite) {
				table[name]->setSaveOnFileWrite(true);
			}
			//return table[name];
		}
	}
// attempt to delete a value from this table also, remove any non-local children who are relying on this value.
// if removing from root, delete all paramaters with this name in table.
	void deleteParameter(const string& name) {
		ASSERT((table.find(name) != table.end()), "  ERROR! :: attempt to remove non-existent \"" << tableNameSpace << name << "\". Exiting");
		if (tableNameSpace == "") {  // if we are in the root table
			(*parameterDocumentation).erase(name);
			// remove parameter from all children
			vector<shared_ptr<ParametersTable>> checklist;
			for (auto c : children) {
				checklist.push_back(c.second);
			}
			while (checklist.size() > 0) {
				if (checklist[0]->table.find(name) != checklist[0]->table.end()) {
					checklist[0]->table.erase(name);
				}
				for (auto c : checklist[0]->children) {
					checklist.push_back(c.second);
				}
				swap(checklist.front(), checklist.back());
				checklist.pop_back();
			}
		} else {  // this is not a root table, only remove parameters from child tables if not local and looking at this table.
			vector<shared_ptr<ParametersTable>> checklist;
			for (auto c : children) {
				if (c.second->table.find(name) != c.second->table.end()) {  // if child has name...
					if (!c.second->table[name]->isLocal()) {  // and childs name is not local...
						c.second->table.erase(name);// delete name and ...
						checklist.push_back(c.second);// add child to checklist
					}  // if child has name and it is local, don't do anything. (if any children are not local, they will be looking here)
				} else {  // if child does not have name...
					checklist.push_back(c.second);// add child to list (so that it's children are checked.
				}
			}
			while (checklist.size() > 0) {
				for (auto c : checklist[0]->children) {
					if (c.second->table.find(name) != c.second->table.end()) {  // if child has name...
						if (!c.second->table[name]->isLocal()) {  // and childs name is not local...
							c.second->table.erase(name);// delete name and ...
							checklist.push_back(c.second);// add child to checklist
						}  // if child has name and it is local, don't do anything. (if any children are not local, they will be looking here)
					} else {  // if child does not have name...
						checklist.push_back(c.second);// add child to list (so that it's children are checked.
					}
				}
				swap(checklist.front(), checklist.back());
				checklist.pop_back();
			}
		}
		table.erase(name);
	}

	// deleteParamatersTable deletes table with nameSpace. Any children of this table are also removed.
	// note, external shared_ptrs to this table (or others in the tree) may still be valid, and will
	// cause the tables to remain active. This should not be a problem, as objects leave scope the tables will
	// auto delete. Also note that shared_ptrs to parameterEntries will still be valid, and this is the desired behavior
	void deleteParamatersTable() {
		while (children.size()>0) {
			children.begin()->second->deleteParamatersTable();
		}
		clearParent();
		rootTable.reset();
		(*parametersTablesRegistry).erase(tableNameSpace);
	}

// set the value in this table (create is needed) (and in root - see set). also assigns usage data, only registered parameters can be saved to files
	void setSaveOnFileWrite(const string& name, bool setting = true) {
		if (table.find(name) != table.end()) {
			table[name]->setSaveOnFileWrite(setting);
		} else {
			cout << "  Warning :: call to setSaveOnFileWrite. parameter \"" << tableNameSpace << name << "\" not found. Ignoring..."<< endl;
		}
	}

	void setDocumentation(const string& name, const string& _documentation) {
		if (table.find(name) != table.end()) {
			(*parameterDocumentation)[name] = _documentation;
		} else {
			cout << "  Warning :: call to set documentation. parameter \"" << tableNameSpace << name << "\" not found. Ignoring..."<< endl;
		}
	}

	template<typename T>
	shared_ptr<ParametersEntry<T>> register_parameter(const string& name, const T& default_value, const string& documentation) {
		if (table.find(name) != table.end()) {  // if this table has entry called name
			cout << "  WARRNING!! Attempting to register \"" << tableNameSpace << name << "\", but it already exists!\n      Updating with new values\n";
		}
		setParameter(name, default_value, rootTable->tableNameSpace);
		setSaveOnFileWrite(name);
		setDocumentation(name, documentation);
		return dynamic_pointer_cast<ParametersEntry<T>>(table[name]);
	}

	vector<shared_ptr<ParametersTable>> getChildren() {
		vector<shared_ptr<ParametersTable>> kids;
		for (auto c : children) {
			kids.push_back(c.second);
		}
		return kids;
	}

	void parametersToSortedList(map<string,vector<string>>& sorted_parameters) {

		sorted_parameters.clear();
		ostringstream build_string;
		string workingNameSpace, workingCategory, workingParameterName, workingParameterType;
		pair<string,string> workingValue;

		for (auto p : table) {
			if (p.second->saveOnWrite()) {
				build_string.str("");
				int i = 0;
				while (p.first[i] != '-' && i < (int)p.first.size()) {  // find "-", separator between category and name
					i++;
				}
				if (i >= (int)p.first.size()-1) {  // if there is not a "-" and at least one character after that... there is a problem.
					cout << "  ERROR :: in ParametersTable::parametersToSortedList(), found misformatted parameter with name \"" << p.first << "\". Possibly missing category. Exiting!"<< endl;
					exit(1);
				}
				workingCategory = p.first.substr(0,i);
				workingParameterName = p.first.substr(i+1,p.first.size());

				workingParameterType = getParameterType(p.first);

				//WRITE GET STRING OF VALUE!
				workingValue = p.second->getStringValuePair();
				build_string << fixed << workingParameterName << " = " << workingValue.first;

				int size = build_string.str().size();
				while (size < 40) {
					build_string << " ";
					size++;
				}
				build_string << " # (" << workingValue.second << ") " << (*parameterDocumentation)[p.first];
				sorted_parameters[workingCategory].push_back(build_string.str());
			}
		}

		for (auto c : sorted_parameters) {
			sort(sorted_parameters[c.first].begin(), sorted_parameters[c.first].end());
		}
	}

	void show(bool showChildren = true) {
		cout << "\nTABLE: ";
		if (parent == nullptr) {
			cout << "(ROOT) " << tableNameSpace;
		} else {
			cout << tableNameSpace;
		}
		cout << "\n    ID: " << getID() << endl;
		if (parent != nullptr) {
			if (parent->parent == nullptr) {
				cout << "    parent name space: (ROOT) " << parent->tableNameSpace << endl;
			} else {
				cout << "    parent name space: " << parent->tableNameSpace << endl;
			}
		} else {
			cout << "    parent name space: NULLPTR (this is ROOT) " << endl;
		}
		cout << "    Children ("<<children.size()<<"):  ";
		for (auto c : children) {
			cout << c.second->tableNameSpace << "  ";
		}
		cout << endl;
		cout << "      Parameters in this table:" << endl;
		for (auto v : table) {  // show all parameters in this table
			cout << "      " << v.first << "  ";
			v.second->show();
			cout << "        documentation: " << (*parameterDocumentation)[v.first] << endl;
		}
		if (showChildren) {
			for (auto c : children) {
				c.second->show();
			}
		}
	}

};

template<typename T>
class ParameterLink {
public:
	string name;
	shared_ptr<ParametersEntry<T>> entry;
	shared_ptr<ParametersTable> table;

	ParameterLink(string _name, shared_ptr<ParametersEntry<T>> _entry, shared_ptr<ParametersTable> _table) :
			name(_name), entry(_entry), table(_table) {
	}

	~ParameterLink() = default;

	T lookup() {
		return entry->get();
	}

	void set(T value) {
		if (entry->isLocal() == true) {
			entry->set(value);
		} else {
			table->setParameter(name, value);
		}
	}

};

class Parameters {
public:
	static shared_ptr<ParametersTable> root;

	template<typename T>
	static shared_ptr<ParameterLink<T>> register_parameter(const string& name, const T& default_value, const string& documentation) {
		if (root == nullptr) {
			root = ParametersTable::makeTable();
		}
		auto entry = root->register_parameter(name, default_value, documentation);
		auto newLink = make_shared<ParameterLink<T>>(name, entry, root);
		return newLink;
	}

	static shared_ptr<ParameterLink<bool>> getBoolLink(const string& name, shared_ptr<ParametersTable> table);
	static shared_ptr<ParameterLink<string>> getStringLink(const string& name, shared_ptr<ParametersTable> table);
	static shared_ptr<ParameterLink<int>> getIntLink(const string& name, shared_ptr<ParametersTable> table);
	static shared_ptr<ParameterLink<double>> getDoubleLink(const string& name, shared_ptr<ParametersTable> table);

//	static shared_ptr<bool> register_parameter(const string& name, const bool& default_value, const string& documentation);
//	static shared_ptr<string> register_parameter(const string& name, const string& default_value, const string& documentation);
//	static shared_ptr<int> register_parameter(const string& name, const int& default_value, const string& documentation);
//	static shared_ptr<double> register_parameter(const string& name, const double& default_value, const string& documentation);

//	template<typename T>
//	static shared_ptr<T> register_parameter(const string& name, const T& default_value, const string& documentation) {
//		if (root == nullptr) {
//			shared_ptr<ParametersTable> root = ParametersTable::makeTable();
//		}
//		return root->register_parameter(name, default_value, documentation);
//	}

//	static shared_ptr<bool> register_parameter(const string& name, const bool& default_value, const string& documentation) {
//		if (root == nullptr){
//			shared_ptr<ParametersTable> root = ParametersTable::makeTable();
//		}
//		return root->register_parameter(name, default_value, documentation);
//	}

	static void parseFullParameterName(const string& fullName, string& nameSpace, string& category, string& parameterName) {
		int i = fullName.size() - 1;
		nameSpace = "";
		category = "";
		parameterName = "";
		string workingString = "";
		while (i > -1) {
			if (fullName[i] == '-') {
				parameterName = workingString;
				workingString = "";
				i--;
				break;
			}
			workingString = fullName[i] + workingString;
			i--;
		}
		while (i > -1) {
			if (i < (int) fullName.size() - 1 && (fullName[i] == ':' && fullName[i + 1] == ':')) {
				category = workingString.substr(1, workingString.size());
				workingString = ":";
				break;
			} else {
				workingString = fullName[i] + workingString;
			}
			i--;
		}

		while (i > -1) {
			workingString = fullName[i] + workingString;
			i--;
		}

		if (category == "") {
			category = workingString;
		} else {
			nameSpace = workingString;
		}
	}

	static void readCommandLine(int argc, const char** argv, unordered_map<string, string>& comand_line_list, vector<string>& fileList) {
		bool temp = false;
		readCommandLine(argc, argv, comand_line_list, fileList, temp);
	}

	static void readCommandLine(int argc, const char** argv, unordered_map<string, string>& comand_line_list, vector<string>& fileList, bool& saveFiles) {
		int argCount = 1;
		while (argCount < argc) {
			if (argv[argCount][0] == '-' && argv[argCount][1] == 'f') {  // denotes that file names will follow
				argCount++;
				if (argCount >= argc) {
					cout << "ERROR! -f found on command line with no file name.\nExiting\n";
					exit(1);
				} else {
					while (argCount < argc && argv[argCount][0] != '-') {
						fileList.push_back(argv[argCount++]);
					}
				}
			} else if (argv[argCount][0] == '-' && argv[argCount][1] == 'p') {  // denotes that parameter/value pairs will follow
				argCount++;
				if (argCount > argc) {
					cout << "ERROR! -p found on command line with no parameters.\nExiting\n";
					exit(1);
				} else {
					while (argCount < argc && argv[argCount][0] != '-') {
						if (argCount + 1 < argc) {
							if (comand_line_list.find(string(argv[argCount])) == comand_line_list.end()) {
								comand_line_list[string(argv[argCount])] = string(argv[argCount + 1]);
							} else {
								cout << "  ERROR :: Parameter \"" << string(argv[argCount]) << "\" is defined more then once on the command line.\nExiting.\n";
								exit(1);
							}
							argCount += 2;
						} else {
							cout << "  ERROR :: Parameter \"" << string(argv[argCount]) << "\" is defined on command line with out a value.\nExiting.\n";
							exit(1);
						}
					}
				}
			} else if (argv[argCount][0] == '-' && argv[argCount][1] == 's') {
				saveFiles = true;
				argCount++;
			}
		}
	}

	static unordered_map<string, string> readParametersFile(string fileName) {
		unordered_map<string, string> config_file_list;
		set<char> nameFirstLegalChars = {  // characters that can be used as the first "letter" of a name
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_' };
		set<char> nameLegalChars = {  // characters that can be used in the body of a name
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '_' };
		set<char> whiteSpaceChars = { ' ', '\t' };

		string line;
		int line_number = 0;
		unsigned int index;
		string parameterName;
		string parameterValue;

		cout << "  - loading file \"" << fileName << "\"\n";

		ifstream configFile(fileName);  // open file named by fileName
		if (configFile.is_open())  // if the file named by fileName can be opened
		{
			string categoryName = "";
			string nameSpace = "";
			while (getline(configFile, line))  // keep loading one line from the file at a time into "line" until we get to the end of the file
			{
				line_number++;
				//cout << "line #: " << line_number << endl;
				index = 0;
				parameterValue = "";
				parameterName = "";
				while (index < line.length() && iswspace(line[index])) {
					index++;
				}

				if (index < line.length() && line[index] == '+') {  //if a line starts with % parse a catagory name
					string newNestingName = "";
					index++;
					if (line[index] != ' ') {
						cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
						cout << "    While reading a nameSpace name, was expecting a blank space after \"+\".\n" << endl;
						exit(1);
					}
					index++;
					// get name (must start with letter)
					if (index < line.length() && isalpha(line[index])) {
						newNestingName += line[index];
						index++;
					} else if (index < line.length()) {  // if the not a name start character
						cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
						cout << "    While reading a nameSpace name, found invalid first character after \"+\".\n" << endl;
						exit(1);
					}
					// get rest of name, must be numbers or letters (no '_'s)
					while (index < line.length() && isalnum(line[index])) {
						newNestingName += line[index];
						index++;
					}
					while (index < line.length() && iswspace(line[index])) {
						index++;
					}
					if (index < line.length() && line[index] != '#') {
						cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
						cout << "    While reading nameSpace name \"" << newNestingName << "\", found unused/invalid characters after nameSpace define outside of comment." << endl;
						exit(1);
					}
					if (newNestingName != "") {
						nameSpace = nameSpace + newNestingName + "::";
					}
					index = line.length();  // move to end of line
				}

				if (index < line.length() && line[index] == '-') {  //if a line starts with - move up one name space
					if (nameSpace == "") {
						cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\". Exiting.\n";
						cout << "    attempt to leave nameSpace, when not in nameSpace" << endl;
						exit(1);
					} else {
						int cutPoint = nameSpace.size() - 2;
						while ((nameSpace[cutPoint] != ':' || nameSpace[cutPoint - 1] != ':') && cutPoint > 0) {
							//cout << cutPoint << " ";
							cutPoint--;
							//cout << cutPoint << endl;
						}
						//cout << "b: " << nameSpace << endl;
						if (cutPoint == 0) {
							nameSpace = "";
						} else {
							nameSpace = nameSpace.substr(0, cutPoint + 1);
						}
						//cout << "a: " << nameSpace << endl;
					}
					index = line.length();  // move to end of line
				}

				if (index < line.length() && line[index] == '%') {  //if a line starts with % parse a catagory name
					categoryName = "";
					index++;
					while (index < line.length() && iswspace(line[index])) {
						index++;
					}
					// get name (must start with letter or "_")
					if (index < line.length() && (isalpha(line[index]) || line[index] == '_')) {
						categoryName += line[index];
						index++;
					} else if (index < line.length()) {  // if the first character is not a letter or "_"
						cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\". Exiting.\n";
						cout << "    While category name found invalid character after \"%\"\n";
						exit(1);
					}
					// get rest of name, must be numbers or letters or '_'s
					while (index < line.length() && (isalnum(line[index]) || line[index] == '_')) {
						categoryName += line[index];
						//cout << newCategoryName << endl;
						index++;
					}
					while (index < line.length() && iswspace(line[index])) {
						index++;
					}
					if (index < line.length() && line[index] != '#') {
						cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\". Exiting.\n";
						cout << "    While reading category name \"" << categoryName << "\", found unused/invalid characters after category define outside of comment." << endl;
						exit(1);
					}
					index = line.length();  // move to end of line
				}

				// move past any leading white space
				while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
					index++;
				}
				// comment can start before name, if it does, we are done with this line... move to end of line
				if (index < line.length() && line[index] == '#') {
					index = line.length();
				}
				// get name (must start with letter or "_")
				if (index < line.length() && nameFirstLegalChars.find(line[index]) != nameFirstLegalChars.end()) {
					parameterName += line[index];
					index++;
				} else if (index < line.length()) {  // if the first non whitespace character is not "#" or a name start character
					parameterName = "BAD_PARAMETER_NAME";  // set the paramterName to tell later error not to print
					index = line.length();
				}
				// get rest of name
				while (index < line.length() && ((nameLegalChars.find(line[index]) != nameLegalChars.end()) || (line[index] == '-') || (line[index] == ':'))) {
					parameterName += line[index];
					index++;
				}
				if (parameterName != "") {
					if (categoryName != "") {
						parameterName = categoryName + "-" + parameterName;
					}

					if (nameSpace != "") {
						parameterName = nameSpace + parameterName;
					}
				}
				//cout << parameterName << "   " << nameSpace << " " << categoryName << "\n";
				// move past white space between name and "="
				while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
					index++;
				}
				// the next character must be "="
				if (index < line.length() && line[index] == '=') {
					index++;
					// move past white space between "=" and value
					while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
						index++;
					}
					// get value : values can be made up of any characters
					while (index < line.length() && whiteSpaceChars.find(line[index]) == whiteSpaceChars.end() && line[index] != '#') {
						parameterValue += line[index];
						index++;
					}
					// move though whitespace till "#" or EoL
					while (index < line.length() && (whiteSpaceChars.find(line[index]) != whiteSpaceChars.end() || line[index] == '#')) {
						if (line[index] == '#') {  // if "#" move to end of line
							index = line.length();
						} else {  // move past whitespace
							index++;
						}
					}
				}
				if (index != line.length()) {  // if not at end of line, there was a problem
					cout << "  ERROR :: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting\n";
					exit(1);
				} else {
					// if there is a name and a value
					if (parameterName != "" && parameterValue != "") {
						if (config_file_list.find(parameterName) == config_file_list.end()) {
							config_file_list[parameterName] = string(parameterValue);
						} else {
							cout << "  - \"" << string(parameterName) << "\" is defined more then once in file: \"" << fileName << "\".\n exiting.\n";
							exit(1);
						}

					} else if (parameterName != "" && parameterValue == "") {  // if there is a name but no value, throw warning
						if (parameterName != "BAD_PARAMETER_NAME") {  // if "BAD_PARAMETER_NAME" then we already printed a warning.
							cout << "  ERROR :: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
							exit(1);
						}
					}
				}
			}
			configFile.close();
		} else {
			cout << "  ERROR! unable to open file \"" << fileName << "\".\nExiting.\n";
			exit(1);
		}
		return config_file_list;
	}

	static void initializeParameters(int argc, const char * argv[]) {

		if (root == nullptr) {
			root = ParametersTable::makeTable();
		}

		unordered_map<string, string> command_line_list;
		vector<string> fileList;

		bool saveFiles = false;
		Parameters::readCommandLine(argc, argv, command_line_list, fileList, saveFiles);

		string workingNameSpace, workingCategory, workingParameterName;

		for (auto fileName : fileList) {  // load all files in order
			unordered_map<string, string> file_list = Parameters::readParametersFile(fileName);
			for (auto i : file_list) {
				parseFullParameterName(i.first, workingNameSpace, workingCategory, workingParameterName);
				ASSERT(workingParameterName != "", "  ERROR! :: reading from file \"" << fileName << "\" found misformatted parameter \"" << i.first << "\"\n  Parameters must have format: [category]-[name] or [name space][category]-[name]");
				string parameterType = root->getParameterType(workingCategory + "-" + workingParameterName);
				if (parameterType == "bool") {
					bool value;
					stringToValue(i.second, value);
					root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
				} else if (parameterType == "string") {
					string value;
					stringToValue(i.second, value);
					root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
				} else if (parameterType == "int") {
					int value;
					stringToValue(i.second, value);
					root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
				} else if (parameterType == "double") {
					double value;
					stringToValue(i.second, value);
					root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
				} else {
					cout << "  ERROR :: while reading file \"" << fileName << "\" found \"" << workingNameSpace + workingCategory + "-" + workingParameterName << ".\n  But \"" << workingCategory + "-" + workingParameterName << "\" is not a registered parameter!\n  Exiting." << endl;
					exit(1);
				}
			}
		}
		for (auto i : command_line_list) {  // load command line parameters last
			parseFullParameterName(i.first, workingNameSpace, workingCategory, workingParameterName);
			ASSERT(workingParameterName != "", "  ERROR! :: reading from command line found misformatted parameter \"" << i.first << "\"\n  Parameters must have format: [category]-[name] or [name space][category]-[name]");
			string parameterType = root->getParameterType(workingCategory + "-" + workingParameterName);
			if (parameterType == "bool") {
				bool value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else if (parameterType == "string") {
				string value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else if (parameterType == "int") {
				int value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else if (parameterType == "double") {
				double value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else {
				cout << "  ERROR :: while reading command line found \"" << workingNameSpace + workingCategory + "-" + workingParameterName << ".\n  But \"" << workingCategory + "-" + workingParameterName << "\" is not a registered parameter!\n  Exiting." << endl;
				exit(1);
			}
		}
		if (saveFiles) {

			Parameters::saveSettingsFiles( { "*" }, { { "settings_organism.cfg", { "GATE*", "GENOME*", "BRAIN*" } }, { "settings_world.cfg", { "WORLD*" } }, { "settings.cfg", { "" } } });
			cout << "Saving config Files and Exiting." << endl;
			exit(0);
		}
	}

	static void saveSettingsFile(const string& nameSpace, stringstream& FILE, vector<string> categoryList, bool alsoChildren = false, int nameSpaceLevel = 0) {
		map<string, vector<string>> sortedParameters;
		root->lookupTable(nameSpace)->parametersToSortedList(sortedParameters);
		if (!root->lookupTable(nameSpace)->neverSave) {
			string currentIndent = "";
			vector<string> nameSpaceParts = nameSpaceToNameParts(nameSpace);

			for (int i = 0; i < nameSpaceLevel; i++) {
				currentIndent += "  ";
				nameSpaceParts.erase(nameSpaceParts.begin());
			}

			if (nameSpaceParts.size() > 0) {
				for (auto p : nameSpaceParts) {
					FILE << currentIndent << "+ " << p.substr(0, p.size() - 2) << "\n";
					nameSpaceLevel++;
					currentIndent += "  ";
				}
			}
			if (categoryList.size() > 0 && categoryList[0] == "-") {
				if (sortedParameters.find("GLOBAL") != sortedParameters.end() && !(find(categoryList.begin(), categoryList.end(), "GLOBAL") != categoryList.end())) {
					FILE << currentIndent << "% GLOBAL" << "\n";
					for (auto parameter : sortedParameters["GLOBAL"]) {
						FILE << currentIndent << "  " << parameter << "\n";
					}
					FILE << "\n";
				}
			} else {  // write parameters to file.
				if (sortedParameters.find("GLOBAL") != sortedParameters.end() && find(categoryList.begin(), categoryList.end(), "GLOBAL") != categoryList.end()) {
					FILE << currentIndent << "% GLOBAL" << "\n";
					for (auto parameter : sortedParameters["GLOBAL"]) {
						FILE << currentIndent << "  " << parameter << "\n";
					}
					FILE << "\n";
				}

			}
			sortedParameters.erase("GLOBAL");

			for (auto group : sortedParameters) {
				bool saveThis = false;
				if (categoryList.size() > 0 && categoryList[0] != "-") {
					for (auto cat : categoryList) {
						if ((int) group.first.size() >= ((int) cat.size()) - 1) {
							if (group.first == cat) {
								saveThis = true;
							} else {
								if ((int) cat.size() > 0 && cat[((int) cat.size()) - 1] == '*') {
									if (group.first.substr(0, cat.size() - 1) == cat.substr(0, cat.size() - 1)) {
										saveThis = true;
									}
								}
							}
						}

					}
				} else {
					saveThis = true;
					for (auto cat : categoryList) {
						if ((int) group.first.size() >= ((int) cat.size()) - 1) {
							if (group.first == cat) {
								saveThis = false;
							} else {
								if ((int) cat.size() > 0 && cat[((int) cat.size()) - 1] == '*') {
									if (group.first.substr(0, cat.size() - 1) == cat.substr(0, cat.size() - 1)) {
										saveThis = false;
									}
								}
							}
						}
					}
				}
				if (saveThis) {
					FILE << currentIndent << "% " << group.first << "\n";
					for (auto parameter : group.second) {
						FILE << currentIndent << "  " << parameter << "\n";
					}
					FILE << "\n";
				}
			}

			if (alsoChildren) {
				vector<shared_ptr<ParametersTable>> checklist = root->lookupTable(nameSpace)->getChildren();
				sort(checklist.begin(), checklist.end());
				for (auto c : checklist) {
					saveSettingsFile(c->getTableNameSpace(), FILE, categoryList, true, nameSpaceLevel);
				}
			}

			while (nameSpaceParts.size() > 0) {
				currentIndent = currentIndent.substr(2, currentIndent.size());
				FILE << currentIndent << "- (" << nameSpaceParts[nameSpaceParts.size() - 1].substr(0, nameSpaceParts[nameSpaceParts.size() - 1].size() - 2) << ")\n";
				nameSpaceParts.pop_back();
			}
			//cout << "  - \"" << fileName << "\" has been created.\n";
		}
	}

	static void saveSettingsFiles(vector<string> nameSpaceList = { "*" }, vector<pair<string, vector<string>>> categoryLists = { {"settings.cfg", {""}}}) {
		bool alsoChildren;
		string fileName;
		vector<string> otherCategoryList;
		for (auto nameSpace : nameSpaceList) {
			for (auto cList : categoryLists) {
				otherCategoryList.insert(otherCategoryList.end(), cList.second.begin(), cList.second.end());
				if ((int)nameSpace.size() > 0 && nameSpace[nameSpace.size()-1] == '*') {
					nameSpace.pop_back();
					alsoChildren = true;
				} else {
					alsoChildren = false;
				}
				fileName = "";
				bool lastCharWasCol = false;
				for (auto c : nameSpace) {
					if (c != ':') {
						fileName += c;
						lastCharWasCol = false;
					} else {
						if (lastCharWasCol == true) {
							fileName += '_';
						}
						lastCharWasCol = true;
					}
				}
				if (fileName != "") {
					fileName = fileName.substr(0,fileName.size()-1)+"-";
				}
				stringstream ss;
				if (cList.second.size() == 1 && cList.second[0]=="") {
					otherCategoryList.insert(otherCategoryList.begin(),"-");
					saveSettingsFile(nameSpace, ss, otherCategoryList, alsoChildren);
				} else {
					saveSettingsFile(nameSpace, ss, cList.second, alsoChildren);
				}
				string workingString = ss.str();
				workingString.erase (remove (workingString.begin(), workingString.end(), ' '), workingString.end());
				workingString.erase (remove (workingString.begin(), workingString.end(), 11), workingString.end());
				bool lastCharEnter = false;
				bool fileEmpty = true;
				for (auto c : workingString) {
					if (c == 10) {
						lastCharEnter = true;
					} else {
						if (lastCharEnter == true) {
							if (!(c == '+' || c == '-' || c == 10)) {
								fileEmpty = false;
							}
						}
						lastCharEnter = false;
					}
				}
				if (!fileEmpty) {
					ofstream FILE(fileName+cList.first);
					FILE << ss.str();
					FILE.close();
				}
			}
		}
	}
};

#endif // __MABE__Parameters__

