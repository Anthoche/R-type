/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Logger
*/

#pragma once

#include <iostream>

#define L_RESET   "\033[0m"
#define L_BLACK   "\033[90m"
#define L_RED     "\033[31m"
#define L_GREEN   "\033[32m"
#define L_YELLOW  "\033[33m"
#define L_BLUE    "\033[34m"

#define LOG_ERROR(msg)   std::cerr << L_BLACK << "[" << L_RED << "ERROR" << L_BLACK "] " << L_RED << msg << L_RESET << std::endl
#define LOG_INFO(msg)    std::cout << L_BLACK << "[" << L_GREEN << "INFO" << L_BLACK "] " << L_RESET << msg << std::endl
#define LOG_DEBUG(msg)   std::cout << L_BLACK << "[" << L_BLUE << "DEBUG" << L_BLACK "] " << L_RESET << msg << std::endl
#define LOG_WARN(msg)   std::cerr << L_BLACK << "[" << L_YELLOW << "WARN" << L_BLACK "] " << L_YELLOW << msg << L_RESET << std::endl
#define LOG(msg)         std::cout << L_RESET << msg << L_RESET << std::endl
