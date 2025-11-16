#include "NetworkManager.h"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>

std::vector<std::string> NetworkManager::getIPv4Addresses() const {
    std::vector<std::string> addrs;
    struct ifaddrs* ifaddr = nullptr;
    
    if (getifaddrs(&ifaddr) == -1) {
        return addrs; // Return empty on error
    }

    collectIPv4Addresses(ifaddr, addrs);
    freeifaddrs(ifaddr);
    
    sortIPAddressesByPriority(addrs);
    return addrs;
}

std::vector<std::string> NetworkManager::getSystemUsers() const {
    std::vector<std::string> users;
    std::ifstream passwd("/etc/passwd");
    
    if (!passwd.is_open()) {
        return users;
    }
    
    std::string line;
    while (std::getline(passwd, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        // Parse passwd format: username:x:uid:gid:comment:home:shell
        std::istringstream iss(line);
        std::string username, x, uid_str;
        
        if (std::getline(iss, username, ':') &&
            std::getline(iss, x, ':') &&
            std::getline(iss, uid_str, ':')) {
            
            try {
                int uid = std::stoi(uid_str);
                // Show users with UID >= 0 (include root and system users that might be valid)
                // Typically real users have UID >= 1000, but on embedded systems this varies
                if (uid >= 0 && uid < 65534) {  // Exclude 'nobody' user (65534)
                    users.push_back(username + " (UID: " + uid_str + ")");
                }
            } catch (...) {
                // Skip invalid entries
            }
        }
    }
    
    return users;
}

void NetworkManager::collectIPv4Addresses(struct ifaddrs* ifaddr, std::vector<std::string>& addrs) const {
    for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!isValidNetworkInterface(ifa)) continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            std::string ipStr = formatIPv4Address(ifa);
            if (!ipStr.empty()) {
                addrs.push_back(ipStr);
            }
        }
    }
}

bool NetworkManager::isValidNetworkInterface(const struct ifaddrs* ifa) const {
    return ifa->ifa_addr != nullptr &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK);
}

std::string NetworkManager::formatIPv4Address(const struct ifaddrs* ifa) const {
    char host[NI_MAXHOST] = {0};
    void* addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
    
    if (inet_ntop(AF_INET, addr, host, sizeof(host))) {
        return std::string(ifa->ifa_name) + ": " + host;
    }
    return "";
}

void NetworkManager::sortIPAddressesByPriority(std::vector<std::string>& addrs) const {
    // Prefer wlan*, then eth*, then usb*, then others for readability
    auto getInterfacePriority = [](const std::string& s) -> int {
        if (s.rfind("wlan", 0) == 0) return 0;
        if (s.rfind("eth", 0) == 0)  return 1;
        if (s.rfind("usb", 0) == 0)  return 2;
        return 3;
    };
    
    std::sort(addrs.begin(), addrs.end(),
              [&](const std::string& a, const std::string& b) {
                  return getInterfacePriority(a) < getInterfacePriority(b);
              });
}
