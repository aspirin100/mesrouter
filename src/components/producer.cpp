#include "components/producer.h"
#include "components/message.h"

Message Producer::ProduceMessage() 
{
    Message msg;

    msg.producer_id = id;
    msg.seq_number = seq_;
    msg.type = producing_msg_type_;

    ++seq_;

    return msg;
}
