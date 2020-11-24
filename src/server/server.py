import mqttudp
import mqttudp.engine as me
mqttudp.engine.send_publish( "test_topic", "Hello, world!" )
def recv_packet(pkt):
    if pkt.ptype != me.PacketType.Publish:
        print( str(pkt.ptype) + ", " + pkt.topic + "\t\t" + str(pkt.addr) )
    return print( pkt.topic+"="+pkt.value+ "\t\t" + str(pkt.addr) )

mqttudp.engine.listen(recv_packet)
