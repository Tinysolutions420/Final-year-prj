// main.cc
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "my_custom_header.h"
#include "my_custom_header.cc"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CustomPacketExample");

int main (int argc, char *argv[])
{
  // Enable logging for this example
  LogComponentEnable ("CustomPacketExample", LOG_LEVEL_INFO);

  // Create two nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Create a point-to-point link between the nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // Install network devices to the nodes
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  // Install the internet stack on the nodes
  InternetStackHelper stack;
  stack.Install (nodes);

  // Assign IP addresses to the devices
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Create a packet sink on node 1 to receive packets
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (interfaces.GetAddress (1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", sinkAddress);
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (20.));

  // Create and configure a socket to send our custom packet
  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
  ns3TcpSocket->Connect (sinkAddress);

  // Create a packet with our custom header
  Ptr<Packet> packet = Create<Packet> ();
  MyCustomHeader header;
  header.SetData("hii");
  packet->AddHeader(header);

  // Send the packet
  ns3TcpSocket->Send (packet);

  // Start the simulation
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

