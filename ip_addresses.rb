'''
 ============================================================================
 Name        : ip_addresses.rb
 Author      : Augustin ROLET
 Version     : 1.0
 Copyright   : Copyright Free
 Description : Network IP addresses tool
 ============================================================================
'''
class IPv4
    # Class to interact with ip IPv4 addresse

    def initialize(ip)
        @ip, @prefix = ip.split("/")
        @prefix = @prefix.to_i
        
        # convert numbers into integers
        @ip = @ip.split(".").map(&:to_i)
        @hostpart = 32 - @prefix
    end
    
    # allow external access
    attr_reader :ip, :hostpart, :prefix, :hostpart

    def assert(expected, actual)
        raise "Assertion error" unless expected == actual
    end
      

    def get_number_ip_available
        # Returns the number of available ips addresses

        if @prefix == 32
            return 1
        elsif @prefix == 31
            return 0
        else
            return 2**@hostpart - 2
        end
    end

    def get_broadcast
        # Returns the broadcast ip address

        if @prefix == 32
            return @ip.join('.')
        end

        n = @prefix / 8
        broadcast = []
        (0...n).each do |i|
            broadcast << @ip[i]
        end

        if @hostpart % 8 != 0
            network_bits = 8 - @hostpart % 8
        else
            network_bits = 0
        end

        next_byte = @ip[n].to_s(2).rjust(8, '0')[0, network_bits]
        # example: 01001010, network_bits = 3
        (network_bits+1...9).each do |i|
            next_byte += "1"
        end        
        # 01001010 becomes 01001111
        broadcast << next_byte.to_i(2) # 2: binary basis

        (n+2...5).each do |i|
            broadcast << 255
        end

        return broadcast.map(&:to_s).join('.')
    end

    def get_network
        # Returns the network ip address

        if @prefix == 32
            return  @ip.join('.')
        end

        n = @prefix / 8
        network = []
        (0...n).each do |i|
            network << @ip[i].to_i
        end

        if @hostpart % 8 != 0
            network_bits = 8 - @hostpart % 8
        else
            network_bits = 0
        end
        next_byte = @ip[n].to_s(2).rjust(8, '0')[0, network_bits]

        (network_bits+1...9).each do |i|
            next_byte += '0'
        end
        network << next_byte.to_i(2)


        (n+2...5).each do |i|
            network <<  0
        end

        return network.map(&:to_s).join('.')
    end

    def get_subnet_mask
        # Returns the subnet mask

        if @prefix == 32
            return "255.255.255.255"
        end
        
        n = @prefix / 8
        subnet_mask = []
        n.times do |i|
            subnet_mask << 255
        end

        remaining_bits = @prefix - n*8
        next_byte = ""

        remaining_bits.times do |i|
            next_byte += "1"
        end
            
        next_byte += '0' * (8-remaining_bits)
        subnet_mask << next_byte.to_i(2)
        
        (n+2..4).each do |i|
            subnet_mask << 0
        end
        return subnet_mask.map(&:to_s).join('.')
    end

    def is_valid
        # Returns true if the IP is not the broadcast or the network IP address, otherwise false
        
        ip = @ip.join('.')
        return get_broadcast != ip && get_network != ip
    end

    def assert_subnet_mask
        # IP inputs and expected subnet masks

        test_cases = [
            ["192.168.10.10/0", "0.0.0.0"],
            ["192.168.10.10/8", "255.0.0.0"],
            ["192.168.10.10/9", "255.128.0.0"],
            ["192.168.10.10/16", "255.255.0.0"],
            ["192.168.10.10/24", "255.255.255.0"],
            ["192.168.10.10/32", "255.255.255.255"],
        ]
        test_cases.each do |ip_input, expected_subnet_mask|
            ip_address = IPv4.new(ip_input)
            assert expected_subnet_mask, ip_address.get_subnet_mask
        end
    end

    def assert_network
        # IP inputs and expected network ips
        
        test_cases = [
            ["192.168.10.10/0", "0.0.0.0"],
            ["192.168.10.10/8", "192.0.0.0"],
            ["192.168.10.10/9", "192.128.0.0"],
            ["192.168.10.10/16", "192.168.0.0"],
            ["192.168.10.10/24", "192.168.10.0"],
            ["192.168.10.10/32", "192.168.10.10"],
        ]

        test_cases.each do |ip_input, expected_network|
            ip_address = IPv4.new(ip_input)
            assert expected_network, ip_address.get_network
        end
    end

    def assert_broadcast
        # IP inputs and expected network ips

        test_cases = [
            ["192.168.10.10/0", "255.255.255.255"],
            ["192.168.10.10/8", "192.255.255.255"],
            ["192.168.10.10/9", "192.255.255.255"],
            ["192.168.10.10/16", "192.168.255.255"],
            ["192.168.10.10/24", "192.168.10.255"],
            ["192.168.10.10/32", "192.168.10.10"],
        ]
            
        test_cases.each do |ip_input, expected_broadcast|
            ip_address = IPv4.new(ip_input)
            assert expected_broadcast, ip_address.get_broadcast
        end
    end

    def assert_number_ip_available
        # IP inputs and expected number ip available

        test_cases = [
            ["192.168.10.10/0", 4294967294],
            ["192.168.10.10/8", 16777214],
            ["192.168.10.10/9", 8388606],
            ["192.168.10.10/16", 65534],
            ["192.168.10.10/24", 254],
            ["192.168.10.10/31", 0],
            ["192.168.10.10/32", 1],
        ]

        test_cases.each do |ip_input, expected_number_ip_available|
            ip_address = IPv4.new(ip_input)
            assert expected_number_ip_available, ip_address.get_number_ip_available
        end
    end
end

if $PROGRAM_NAME == __FILE__
    ip_address_input = "192.168.1.1/24"
    ip_address = IPv4.new(ip_address_input)

    # --- Assertion tests --- #
    ip_address.assert_broadcast
    ip_address.assert_network
    ip_address.assert_number_ip_available
    ip_address.assert_subnet_mask
end