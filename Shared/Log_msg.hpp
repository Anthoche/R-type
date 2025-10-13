/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Log_msg
*/


#pragma once
#include <iostream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#define LOG_ERROR(msg)   std::cerr << RED << "[ERROR] " << msg << RESET << std::endl
#define LOG_INFO(msg)    std::cout << GREEN << "[INFO] " << msg << RESET << std::endl
#define LOG_DEBUG(msg)   std::cout << YELLOW << "[DEBUG] " << msg << RESET << std::endl
#define LOG(msg)         std::cout << BLUE << msg << RESET << std::endl