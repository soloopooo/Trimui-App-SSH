#pragma once

#include <string>
#include <vector>

struct ifaddrs;

class NetworkManager {
public:
    std::vector<std::string> getIPv4Addresses() const;
    std::vector<std::string> getSystemUsers() const;

private:
    void collectIPv4Addresses(struct ifaddrs* ifaddr, std::vector<std::string>& addrs) const;
    bool isValidNetworkInterface(const struct ifaddrs* ifa) const;
    std::string formatIPv4Address(const struct ifaddrs* ifa) const;
    void sortIPAddressesByPriority(std::vector<std::string>& addrs) const;
};
