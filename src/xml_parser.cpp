#include "headers/xml_parser.hpp"
#include "headers/utils.hpp"
#include <iostream>
#include <tuple>
#include <stack>

void Node::updatePointers(){
    if(std::size(children) != 0){
        for(auto & child : children){
            child.parent = this;
            child.updatePointers();
        }
    }
}

std::tuple<std::string , std::map<std::string , std::string>> extract_datas(std::string str){

    if (str.find(" ") == -1){
        if(str[str.length()-1] == '/')
            str.erase(str.length()-1);
        return {str , {}};
    }
    else {
        std::string tag = str.substr(0,str.find(" "));
        int index = str.find(" ");

        std::map<std::string , std::string> attributes;

        while (index != -1){
            
            int eindex = str.find('=' , index);
            std::string key = str.substr(index , eindex-index);

            std::string value;

            int quote = str.find('"' , index);
            int apostrophe = str.find("'" , index);

            if ((apostrophe == -1) or ((quote != -1) and (quote < apostrophe))){
                value = str.substr(quote+1 , str.find('"' , quote+1)-1-quote);
                index = str.find(' ' , str.find('"' , quote+1));
            }
            else {
                value = str.substr(apostrophe+1 , str.find("'" , apostrophe+1)-1-apostrophe);
                index = str.find(' ' , str.find("'" , apostrophe+1));
            }

            attributes.insert({strip(key) , value});
        
        }

        return {tag , attributes};
    }

}

bool Node::isEmpty() const{
    return (std::size(children) == 0);
}

std::vector<const Node*> Node::postfixeParcours() const{
    std::vector<const Node*> list;
    if(std::size(children) != 0){
        for(auto & child : children){
            auto clist = child.postfixeParcours();
            list.insert(list.end() , clist.begin() , clist.end());
        }
    }
    list.push_back(this);

    return list;
}

unsigned int Node::getChildNumber() const {
    return std::size(children);
}

Node::Node(bool orphean){
    m_orphean = orphean;
}

void Node::addchild(Node& t){
    this->children.push_back(t);
}

Node& Node::operator[] (std::size_t idx){
    return this->children[idx];
}

const std::vector<Node> Node::get_children() const{
    return this->children;
}

bool Node::isOrphean() const {
    return m_orphean;
}

Tree parse(std::string fpath){

    std::ifstream fs {fpath};

    if(!fs)
        throw std::runtime_error("XML_PARSER PATH ERROR : Could not open file at "+fpath+"\n\tPossible issue : Incorrect path");
    std::string lines;
    std::string line;

    // extracting xml file content
    while(std::getline(fs , line)){
        lines += line;
    }

    int sindex = lines.find('<');
    std::vector<std::string> parts;
    std::vector<std::string> contents;

    while(sindex != -1){
        
        std::string part = "";
        auto findex = sindex;

        bool correct_part = false;
        while(!correct_part){
            findex = lines.find('>' , findex+1);
            part = lines.substr(sindex+1 , findex-1-sindex);
            if(!(lines.find('>' , findex+1) < lines.find('<' , findex+1))){
                correct_part = true;
                sindex = lines.find('<',findex+1);
            }
        }

        if(std::count(part.begin() , part.end() , '<') > 0){
            throw std::runtime_error("XML_PARSER SYNTAX ERROR : Found extra '<' in a tag");
        } else if (part.length() == 0){
            throw std::runtime_error("XML_PARSER SYNTAX ERROR : Found empty tag");
        }

        parts.push_back(strip(part));
        // extracting content between tags
        if (sindex != -1 && part.at(0) != '?'){
            auto content = strip(lines.substr(findex+1 , sindex-1-findex) , true , false);
            contents.push_back(content);
        }
    }
    // remove special tags
    parts.erase(std::remove_if(parts.begin() , parts.end() , [](std::string part){return part.at(0) == '?';}) , parts.end());

    std::vector<unsigned int> node_heights;
    std::vector<Node> tree_struct;

    unsigned int index {0};

    // Preparing node height , I need it when I will compact the list of node

    for (auto &part : parts){
        if(part.at(0) == '/'){
            index--;
            continue;
        }
        node_heights.push_back(index);

        if(part.at(part.size()-1) != '/'){
            index++;
        }
        
    }

    // attributing tags and attributes to nodes

    for (auto &part : parts){
        if(tree_struct.empty()){
            auto [tag , attributes] = extract_datas(part);
            Node tree {};
            tree.tag = tag;
            tree.attributes = attributes;
            tree_struct.push_back(tree);
        }
        else{
            if(part[0] == '/'){
                continue;
            }

            auto [tag , attributes] = extract_datas(part);
            Node child {(part[part.length()-1] == '/')};
            child.tag = tag;
            child.attributes = attributes;

            tree_struct.push_back(child);
        }
        
    }

    // packing contents , and attribute each content to a node
 
    std::stack<Node*> treeTracker;
    int tindex = 0;
    for (int i = 0; i < std::size(contents);i++){
        if(parts[i].back() != '/'){
            if(parts[i].front() == '/'){
                treeTracker.pop();
                treeTracker.top()->content += contents[i];
                continue;
            }
            Node * tree = &tree_struct[tindex];
            treeTracker.push(tree);
            tree->content += contents[i];
            tindex ++;
        }
        else{
            treeTracker.top()->content += contents[i];
        }
    }

    for(auto & node : tree_struct){
        std::cout << node.tag << " : " << std::endl;
        std::cout << node.content << std::endl;
    }

    Node root{};
    unsigned int hindex {*std::max_element(node_heights.begin() , node_heights.end())};
    auto findex {std::begin(node_heights)};

    // Finalisation of the tree
    while(hindex != 0){
        findex = std::find(std::begin(node_heights) , std::end(node_heights),hindex);
        if(findex != std::end(node_heights)){
            int dist = std::distance(std::begin(node_heights),findex);
            auto tindex {tree_struct.begin()+dist-1};
            (*tindex).addchild(*(tindex+1));
            tindex++;
            node_heights.erase(findex);
            tree_struct.erase(tindex);
        }
        else{
            hindex--;
        }
    }

    root = tree_struct[0];
    auto tree = Tree(root);
    tree.root.updatePointers();
    return tree;
}

std::vector<const Node*> Node::findall(std::string tag) const {
    auto nodeList = postfixeParcours();

    std::vector<const Node * > foundItems {};

    for(auto node : nodeList){
        if(node->tag == tag){
            foundItems.push_back(node);
        }
    }

    return foundItems;
}

Tree::Tree(Node root){
    this->root = root;
}

