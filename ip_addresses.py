'''
 ============================================================================
 Name        : ip_addresses.c
 Author      : Augustin ROLET
 Version     : 1.0
 Copyright   : Copyright Free
 Description : Network IP addresses tool
 ============================================================================
'''
class IPv4:
    """Class to interact with ip IPv4 addresses"""

    def __init__(self, ip: str):
        # 32 bits = IPv4
        self.ip, self.prefix = ip.split("/")
        self.prefix = int(self.prefix)
        self.ip = self.ip.split(".")
        # convert ip into 4 bytes of integers
        for byte in self.ip:
            byte = int(byte)
        self.hostpart = 32 - self.prefix

    def get_number_ip_available(self) -> int:
        """Returns the number of available ips addresses"""
        if self.prefix == 32:
            return 1
        elif self.prefix == 31:
            return 0
        return 2**self.hostpart - 2

    def get_broadcast(self) -> str:
        """Returns the broadcast ip address"""
        if self.prefix == 32:
            return ".".join(map(str, self.ip))
        # the first n bytes are fixed
        n = self.prefix // 8
        broadcast = []
        for i in range(n):
            broadcast.append(int(self.ip[i]))
        # number of bits for the network on the (n+1)byte        
        # this case threat when the prefix is a multiple of 8
        # it avoids issues when translating into binary
        network_bits = 8 - self.hostpart % 8 if not self.hostpart % 8 == 0 else 0
        # byte the need to be changed for the broadcast
        # first we ensure that the decimal number is an int
        # then we take the binary form but without "Ob"
        # finally we fill with zeros if needed
        # we keep only the fixed bits and let the loop fill the rest with ones
        next_byte = bin(int(self.ip[n]))[2:].zfill(8)[:network_bits]
        for i in range(network_bits+1, 9):
            next_byte += "1"
        # convert the bits of the (n+1)th byte in decimal
        broadcast.append(int(next_byte, 2))

        # finish by filling with 255 the remaining bytes
        for i in range(n+2, 5): # skips the threated bytes
            broadcast.append(255)
        
        return ".".join(map(str, broadcast))
    
    def get_network(self) -> str:
        """Returns the network ip address"""
        if self.prefix == 32:
            return ".".join(map(str, self.ip))
        # the first n bytes are fixed
        n = self.prefix // 8
        network = []
        for i in range(n):
            network.append(int(self.ip[i]))
        # number of bits for the network on the (n+1)byte        
        # this case threat when the prefix is a multiple of 8
        # it avoids issues when translating nto binary
        network_bits = 8 - self.hostpart % 8 if not self.hostpart % 8 == 0 else 0
        # byte the need to be changed for the broadcast
        # first we ensure that the decimal number is an int
        # then we take the binary form but without "Ob"
        # finally we fill with zeros if needed
        # we keep only the fixed bits and let the loop fill the rest with ones
        next_byte = bin(int(self.ip[n]))[2:].zfill(8)[:network_bits]
        for i in range(network_bits+1, 9):
            next_byte += "0"
        # convert the bits of the (n+1)th byte in decimal
        network.append(int(next_byte, 2))

        # finish by filling with 255 the remaining bytes
        for i in range(n+2, 5): # skips the threated bytes
            network.append(0)
        
        return ".".join(map(str, network))

    def get_subnet_mask(self) -> str:
        """Returns the subnet mask"""
        if self.prefix == 32:
            # /32 subnet mask for a single host
            return "255.255.255.255"
        # the first n bytes are full of ones
        n = self.prefix // 8
        subnet_mask = []
        for i in range(n):
            subnet_mask.append(255)

        remaining_bits = self.prefix - n*8
        next_byte = ""
        for i in range(remaining_bits):
            next_byte += "1"
        # line where we finish to fill with ones
        # 8 - remaining_bits = bits to fill with 0
        next_byte += (8-remaining_bits) * "0"
        subnet_mask.append(int(next_byte, 2))
        
        # fill with 0 for the remaining bytes
        for i in range(n+2, 5):
            subnet_mask.append(0)

        return ".".join(map(str, subnet_mask))
    
    def is_valid(self) -> bool:
        """Returns True if the ip is not the broadcast and not the network ip address else False"""
        ip = ".".join(map(str, self.ip))
        return self.get_broadcast() != ip and not self.get_network() != ip
    
    def assert_subnet_mask(self):
        # IP inputs and expected subnet masks
        test_cases = [
            ("192.168.10.10/0", "0.0.0.0"),
            ("192.168.10.10/8", "255.0.0.0"),
            ("192.168.10.10/9", "255.128.0.0"),
            ("192.168.10.10/16", "255.255.0.0"),
            ("192.168.10.10/24", "255.255.255.0"),
            ("192.168.10.10/32", "255.255.255.255"),
        ]
        for ip_input, expected_subnet_mask in test_cases:
            ip_address = IPv4(ip_input)
            assert ip_address.get_subnet_mask() == expected_subnet_mask

    def assert_network(self):
        # IP inputs and expected network ips
        test_cases = [
            ("192.168.10.10/0", "0.0.0.0"),
            ("192.168.10.10/8", "192.0.0.0"),
            ("192.168.10.10/9", "192.128.0.0"),
            ("192.168.10.10/16", "192.168.0.0"),
            ("192.168.10.10/24", "192.168.10.0"),
            ("192.168.10.10/32", "192.168.10.10"),
        ]
        for ip_input, expected_network in test_cases:
            ip_address = IPv4(ip_input)
            assert ip_address.get_network() == expected_network

    def assert_broadcast(self):
        # IP inputs and expected network ips
        test_cases = [
            ("192.168.10.10/0", "255.255.255.255"),
            ("192.168.10.10/8", "192.255.255.255"),
            ("192.168.10.10/9", "192.255.255.255"),
            ("192.168.10.10/16", "192.168.255.255"),
            ("192.168.10.10/24", "192.168.10.255"),
            ("192.168.10.10/32", "192.168.10.10"),
        ]
        for ip_input, expected_broadcast in test_cases:
            ip_address = IPv4(ip_input)
            assert ip_address.get_broadcast() == expected_broadcast

    def assert_number_ip_available(self):
        # IP inputs and expected number ip available
        test_cases = [
            ("192.168.10.10/0", 4294967294),
            ("192.168.10.10/8", 16777214),
            ("192.168.10.10/9", 8388606),
            ("192.168.10.10/16", 65534),
            ("192.168.10.10/24", 254),
            ("192.168.10.10/31", 0),
            ("192.168.10.10/32", 1),
        ]
        for ip_input, expected_number_ip_available in test_cases:
            ip_address = IPv4(ip_input)
            assert ip_address.get_number_ip_available() == expected_number_ip_available

if __name__ == "__main__":
    #ip_address_input = input("Enter your ip adress and its prefix: ")
    ip_address_input = "192.168.1.1/24"
    ip_address = IPv4(ip_address_input)

    # --- Assertion tests --- #
    ip_address.assert_broadcast()
    ip_address.assert_network()
    ip_address.assert_number_ip_available()
    ip_address.assert_subnet_mask()