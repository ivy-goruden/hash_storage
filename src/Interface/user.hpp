#ifndef S21_INTERFACE_USER_HPP
#define S21_INTERFACE_USER_HPP
#include "../include.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <string>
#include "absl/strings/numbers.h"
#include <cstdio>
class User_Filter{
    public:
        static absl::StatusOr<User_Filter> Create(vector<string> params){
            if (params.size() != 5) return absl::InvalidArgumentError("Wrong amount of arguments");
            string lN = params[0];
            string fN = params[1];
            string year = params[2];
            string c = params[3];
            string coins = params[4];
            int yearVal;
            if (year!= "-" &&!absl::SimpleAtoi(year, &yearVal)) {
                return absl::InvalidArgumentError("param[2] is not a valid integer");
            }
            int coinsVal;
            if (coins!= "-" &&!absl::SimpleAtoi(coins, &coinsVal)) {
                return absl::InvalidArgumentError("param[4] is not a valid integer");
            }
            return User_Filter(lN, fN, year, c, coins);

        }
        ~User_Filter() = default;
    public:
     optional<string> lastName;
     optional<string> firstName;
     optional<int> yearOfBirth;
     optional<string> city;
     optional<int> coinsQuantity;
    private:
        User_Filter(string lN, string fN, string year, string c, string coins){
        if (lN!="-"){
            lastName = lN;
        }
        if (fN!="-"){
            firstName = fN;
        }
        if (year!="-"){
            int yearVal;
            bool result = absl::SimpleAtoi(year, &yearVal);
            yearOfBirth = yearVal;
        }
        if (c!="-"){
            city = c;
        }
        if (coins!="-"){
            int coinsVal;
            bool result = absl::SimpleAtoi(coins, &coinsVal);
            coinsQuantity = coinsVal;
        }
    }

};

class User{
    public:
     static absl::StatusOr<User> Create(vector<string> params){
        if (params.size() !=5){
            return absl::InvalidArgumentError("Wrong amount of arguments");
        }
        int year;
        if (!absl::SimpleAtoi(params[2], &year)) {
            return absl::InvalidArgumentError("params[2] is not a valid integer");
        }
        int coins;

        if (!absl::SimpleAtoi(params[4], &coins)) {
            return absl::InvalidArgumentError("params[4] is not a valid integer");
        }
        
        return User(params[0], params[1], year, params[3], coins);

     }
     ~User() = default;
     bool operator==(User_Filter other) const{
        if (lastName != other.lastName && other.lastName.has_value()){
            return false;
        }
        if (firstName != other.firstName && other.firstName.has_value()){
            return false;
        }
        if (yearOfBirth != other.yearOfBirth && other.yearOfBirth.has_value()){
            return false;
        }
        if (city != other.city && other.city.has_value()){
            return false;
        }
        if (coinsQuantity != other.coinsQuantity && other.coinsQuantity.has_value()){
            return false;
        }
        return true;
     }
     void operator=(User_Filter other){
        if (other.lastName.has_value()){
            lastName = other.lastName.value();
        }
        if (other.firstName.has_value()){
            firstName = other.firstName.value();
        }
        if (other.yearOfBirth.has_value()){
            yearOfBirth = other.yearOfBirth.value();
        }
        if (other.city.has_value()){
            city = other.city.value();
        }
        if (other.coinsQuantity.has_value()){
            coinsQuantity = other.coinsQuantity.value();
        }
     }
     
    public:
     string lastName;
     string firstName;
     int yearOfBirth;
     string city;
     int coinsQuantity;
    
    private:
     User(string lN, string fN, int year, string c, int coins):
     lastName(lN),
     firstName(fN),
     yearOfBirth(year),
     city(c),
     coinsQuantity(coins){}
};

#endif