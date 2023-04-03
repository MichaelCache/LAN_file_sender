#include "dhcp_server.h"

#include <Windows.h>
#include <dhcpsapi.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "dhcpsapi.lib")

WCHAR serverIpAddress[] = L"192.168.1.1";

void dhcpServerStart() {
  // Initialize the DHCP server
  DWORD dwError = 0;
  DHCPDS_SERVER serverInfo = {0};
  serverInfo.ServerName = serverIpAddress;
  dwError = DhcpAddServer(0, NULL, &serverInfo, NULL, NULL);
  if (dwError != ERROR_SUCCESS) {
    //     printf("Error adding DHCP server: %d\n", dwError);
    //     // return 1;
  }

  // create a subnet
  DHCP_IP_ADDRESS subnetAddress = inet_addr("192.168.1.0");
  DHCP_IP_ADDRESS subnetMask = inet_addr("255.255.255.0");
  WCHAR subnetName[] = L"LAN file share Subnet";
  //   WCHAR subnetComment[] = L"A subnet for LAN file share";

  DHCP_SUBNET_INFO subnetInfo = {0};
  subnetInfo.SubnetAddress = subnetAddress;
  subnetInfo.SubnetMask = subnetMask;
  subnetInfo.SubnetName = subnetName;

  auto dwResult = DhcpCreateSubnet(serverIpAddress, subnetAddress, &subnetInfo);

  if (dwResult != ERROR_SUCCESS) {
    // handle error
  }

  // create a range
  DHCP_IP_ADDRESS rangeStartAddress = inet_addr("192.168.1.100");
  DHCP_IP_ADDRESS rangeEndAddress = inet_addr("192.168.1.200");
  WCHAR rangeComment[] = L"A range for my subnet";

  DHCP_IP_RANGE range;
  range.StartAddress = rangeStartAddress;
  range.EndAddress = rangeEndAddress;

  DHCP_SUBNET_ELEMENT_DATA_V4 elementData;
  elementData.ElementType = DhcpIpRanges;
  elementData.Element.IpRange = &range;

  dwResult =
      DhcpAddSubnetElementV4(serverIpAddress, subnetAddress, &elementData);

  if (dwResult != ERROR_SUCCESS) {
    // handle error
  }
}

void dhcpServerStop() {
  DHCPDS_SERVER serverInfo = {0};
  serverInfo.ServerName = serverIpAddress;
  DWORD dwError = DhcpDeleteServer(0, NULL, &serverInfo, NULL, NULL);
  if (dwError != ERROR_SUCCESS) {
    // printf("Error deleting DHCP server: %d\n", dwError);
    // return 1;
  }
}