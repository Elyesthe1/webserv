#pragma once
#include <iostream>
#include <vector>
struct Route 
{
    std::string root;                      
    std::string path;                      
    std::string index;                    
    std::string redirection;              
    bool autoindex;                       
    std::vector<std::string> methods;
    std::string upload;          
    std::string cgi_extension;
    Route();
};