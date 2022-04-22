/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "ns3/mobility-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int main()
{
    bool verbose = true;
    uint32_t nCsma = 4;

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    NodeContainer p2pNodes_ccc_csma;
    p2pNodes_ccc_csma.Create(2);

    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes_ccc_csma.Get(1));
    csmaNodes.Create(nCsma);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes_ccc_csma);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    InternetStackHelper stack;
    stack.Install(p2pNodes_ccc_csma.Get(0));
    stack.Install(csmaNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    // CCC router with cnc router

    NodeContainer p2pNodes_cnc_ccc;
    p2pNodes_cnc_ccc.Add(p2pNodes_ccc_csma.Get(0));
    p2pNodes_cnc_ccc.Create(1);

    PointToPointHelper pointToPoint_cnc_ccc;
    pointToPoint_cnc_ccc.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
    pointToPoint_cnc_ccc.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices_cnc_ccc;
    p2pDevices_cnc_ccc = pointToPoint_cnc_ccc.Install(p2pNodes_cnc_ccc);

    stack.Install(p2pNodes_cnc_ccc.Get(1));

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces_cnc_ccc;
    p2pInterfaces_cnc_ccc = address.Assign(p2pDevices_cnc_ccc);

    //

    // CNC router with server 1
    // server 1 is for FTP transfer of certificates with csma nodes

    NodeContainer p2pNodes_cnc_ser1;
    p2pNodes_cnc_ser1.Add(p2pNodes_cnc_ccc.Get(1));
    p2pNodes_cnc_ser1.Create(1);

    PointToPointHelper pointToPoint_cnc_ser1;
    pointToPoint_cnc_ser1.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint_cnc_ser1.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices_cnc_ser1;
    p2pDevices_cnc_ser1 = pointToPoint_cnc_ser1.Install(p2pNodes_cnc_ser1);

    stack.Install(p2pNodes_cnc_ser1.Get(1));

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces_cnc_ser1;
    p2pInterfaces_cnc_ser1 = address.Assign(p2pDevices_cnc_ser1);

    //

    // CNC router with server 2
    // server2 is for CBR with laptops in wifi

    NodeContainer p2pNodes_cnc_ser2;
    p2pNodes_cnc_ser2.Add(p2pNodes_cnc_ccc.Get(1));
    p2pNodes_cnc_ser2.Create(1);

    PointToPointHelper pointToPoint_cnc_ser2;
    pointToPoint_cnc_ser2.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint_cnc_ser2.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices_cnc_ser2;
    p2pDevices_cnc_ser2 = pointToPoint_cnc_ser2.Install(p2pNodes_cnc_ser2);

    stack.Install(p2pNodes_cnc_ser2.Get(1));

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces_cnc_ser2;
    p2pInterfaces_cnc_ser2 = address.Assign(p2pDevices_cnc_ser2);

    //

    // CCC router with wifi

    NodeContainer p2pNodes_ccc_wifi;
    p2pNodes_ccc_wifi.Add(p2pNodes_ccc_csma.Get(0));
    p2pNodes_ccc_wifi.Create(1);

    PointToPointHelper pointToPoint_ccc_wifi;
    pointToPoint_ccc_wifi.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint_ccc_wifi.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices_ccc_wifi;
    p2pDevices_ccc_wifi = pointToPoint_ccc_wifi.Install(p2pNodes_ccc_wifi);

    stack.Install(p2pNodes_ccc_wifi.Get(1));

    address.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces_ccc_wifi;
    p2pInterfaces_ccc_wifi = address.Assign(p2pDevices_ccc_wifi);

    //

    // wifi

    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(3);
    NodeContainer wifiApNode = p2pNodes_ccc_wifi.Get(1);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "ActiveProbing", BooleanValue(false));

    NetDeviceContainer staDevices;
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevices;
    apDevices = wifi.Install(phy, mac, wifiApNode);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(5.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(wifiStaNodes);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);

    stack.Install(wifiStaNodes);

    address.SetBase("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer wifiNodeInterfaces;

    wifiNodeInterfaces = address.Assign(staDevices);
    address.Assign(apDevices);

    //

    /* normal pinging

    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(p2pNodes_cnc_ser2.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(100.0));

    UdpEchoClientHelper echoClient(p2pInterfaces_cnc_ser2.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(3));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    // ApplicationContainer clientApps = echoClient.Install(csmaNodes.Get(1));
    ApplicationContainer clientApps = echoClient.Install(wifiStaNodes.Get(1));

    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(100.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    pointToPoint.EnablePcapAll("second");
    csma.EnablePcap("second", csmaDevices.Get(1), true);

    Simulator::Stop(Seconds(100));
    Simulator::Run();
    Simulator::Destroy();

    */

    // for FTP  with server 1 and csma nodes

    // FTP server

    uint16_t PORT = 9;
	BulkSendHelper source("ns3::TcpSocketFactory",
						  InetSocketAddress(csmaInterfaces.GetAddress(1), PORT));
	// Set the amount of data to send in bytes.  Zero is unlimited.
	source.SetAttribute("MaxBytes", UintegerValue(30));
	ApplicationContainer sourceApps = source.Install(p2pNodes_cnc_ser1.Get(1));
	sourceApps.Start(Seconds(1.0));
	sourceApps.Stop(Seconds(100.0));

	// client
	PacketSinkHelper sink("ns3::TcpSocketFactory",
						  InetSocketAddress(Ipv4Address::GetAny(), PORT));
	ApplicationContainer sinkApps = sink.Install(csmaNodes.Get(1));
	sinkApps.Start(Seconds(0.0));
	sinkApps.Stop(Seconds(100.0));

	AsciiTraceHelper ascii;
	pointToPoint_cnc_ser1.EnableAsciiAll(ascii.CreateFileStream("assgn.tr"));
	pointToPoint_cnc_ser1.EnablePcapAll("assgn", false);

    //

    // for CBR transmission with server 2 and wifi laptops

    // CBR
	Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(250));
	Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("5kb/s"));

	// Server
	OnOffHelper serverHelper("ns3::TcpSocketFactory", InetSocketAddress(wifiNodeInterfaces.GetAddress(1), PORT));
	serverHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	serverHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	ApplicationContainer serverApps;
	serverApps.Add(serverHelper.Install(p2pNodes_cnc_ser2.Get(1)));

	// Client
	PacketSinkHelper sinkCBR("ns3::TcpSocketFactory",
							 InetSocketAddress(Ipv4Address::GetAny(), PORT));
	ApplicationContainer sinkAppsCBR = sinkCBR.Install(wifiStaNodes.Get(1));
	sinkAppsCBR.Start(Seconds(3.0));
	sinkAppsCBR.Stop(Seconds(100.0));

    
    //

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(100.0));
    Simulator::Run();
    Simulator::Destroy();
    Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(sinkApps.Get(0));
    std::cout << "Total Bytes Received: " << sink1->GetTotalRx() << std::endl;

    return 0;
}