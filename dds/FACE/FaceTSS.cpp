#include "FACE/TS.hpp"
#include "FaceTSS.h"
#include "config/Parser.h"

#include "dds/DCPS/Service_Participant.h"
#include "dds/DCPS/Registered_Data_Types.h"
#include "dds/DCPS/Marked_Default_Qos.h"

#include <string>
#include <map>

namespace FACE {
namespace TS {

using OpenDDS::FaceTSS::config::ConnectionSettings;
using OpenDDS::FaceTSS::config::TopicSettings;
using OpenDDS::FaceTSS::config::QosSettings;

namespace {
  OpenDDS::FaceTSS::config::Parser parser;

  RETURN_CODE_TYPE create_opendds_entities(CONNECTION_ID_TYPE connectionId,
                                           const ::DDS::DomainId_t domainId,
                                           const char* topic,
                                           const char* type,
                                           CONNECTION_DIRECTION_TYPE dir,
                                           QosSettings& qos);
}

using OpenDDS::FaceTSS::Entities;

void Initialize(const CONFIGURATION_RESOURCE configuration_file,
                RETURN_CODE_TYPE& return_code)
{
  int status = parser.parse(configuration_file);
  if (status != 0) {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("(%P|%t) ERROR: Initialize() ")
               ACE_TEXT("Parser::parse () returned %d\n"),
               status));
    return_code = INVALID_PARAM;
  } else {
    return_code = NO_ERROR;
  }
}

void Create_Connection(const CONNECTION_NAME_TYPE connection_name,
                       MESSAGING_PATTERN_TYPE pattern,
                       CONNECTION_ID_TYPE& connection_id,
                       CONNECTION_DIRECTION_TYPE& connection_direction,
                       MESSAGE_SIZE_TYPE& max_message_size,
                       RETURN_CODE_TYPE& return_code)
{
  return_code = NO_ERROR;

  if (pattern != PUB_SUB) {
    return_code = INVALID_CONFIG;
    return;
  }

  ConnectionSettings connection;
  TopicSettings topic;
  QosSettings qos;

  // Find connection
  if (!parser.find_connection(connection_name, connection)) {
    // Find topic
    if (!parser.find_topic(connection.topic_name_, topic)) {
      // If qos name was specified for the connection
      if (strlen(connection.qos_name_)) {
        // Find Qos by specified name
        if (parser.find_qos(connection.qos_name_, qos)) {
          return_code = INVALID_CONFIG;
        }
      } else {
        // Find Qos by default (topic) name
        parser.find_qos(connection.topic_name_, qos);
        // Ignore result, not required
      }
    } else {
      return_code = INVALID_CONFIG;
    }
  } else {
    return_code = INVALID_CONFIG;
  }

  if (return_code == NO_ERROR) {
    // Copy out parameters
    connection_id = connection.connection_id_;
    connection_direction = connection.direction_;
    max_message_size = topic.max_message_size_;
    
    return_code = create_opendds_entities(connection_id,
                                          connection.domain_id_,
                                          connection.topic_name_,
                                          topic.type_name_,
                                          connection_direction,
                                          qos);
    return;
  }

  return_code = INVALID_PARAM;
}

void Get_Connection_Parameters(CONNECTION_NAME_TYPE& connection_name,
                               CONNECTION_ID_TYPE& connection_id,
                               TRANSPORT_CONNECTION_STATUS_TYPE& status,
                               RETURN_CODE_TYPE& return_code)
{
  //TODO: implement
  return_code = NO_ERROR;
}

void Destroy_Connection(CONNECTION_ID_TYPE connection_id,
                        RETURN_CODE_TYPE& return_code)
{
  Entities& entities = *Entities::instance();
  std::map<int, ::DDS::DataWriter_var>& writers = entities.writers_;
  std::map<int, ::DDS::DataReader_var>& readers = entities.readers_;

  ::DDS::DomainParticipant_var dp;
  if (writers.count(connection_id)) {
    const ::DDS::Publisher_var pub = writers[connection_id]->get_publisher();
    writers.erase(connection_id);
    dp = pub->get_participant();

  } else if (readers.count(connection_id)) {
    const ::DDS::Subscriber_var sub = readers[connection_id]->get_subscriber();
    readers.erase(connection_id);
    dp = sub->get_participant();
  }

  if (!dp) {
    return_code = INVALID_PARAM;
    return;
  }

  dp->delete_contained_entities();
  const ::DDS::DomainParticipantFactory_var dpf = TheParticipantFactory;
  dpf->delete_participant(dp);
  return_code = NO_ERROR;
}

namespace {
  RETURN_CODE_TYPE create_opendds_entities(CONNECTION_ID_TYPE connectionId,
                                           const ::DDS::DomainId_t domainId,
                                           const char* topicName,
                                           const char* type,
                                           CONNECTION_DIRECTION_TYPE dir,
                                           QosSettings& qos_settings) {
#ifdef DEBUG_OPENDDS_FACETSS
    OpenDDS::DCPS::set_DCPS_debug_level(8);
    OpenDDS::DCPS::Transport_debug_level = 5;
    TheServiceParticipant->set_BIT(false);
#endif

    const ::DDS::DomainParticipantFactory_var dpf = TheParticipantFactory;
    if (!dpf) return INVALID_PARAM;

    const ::DDS::DomainParticipant_var dp =
      dpf->create_participant(domainId, PARTICIPANT_QOS_DEFAULT, 0, 0);
    if (!dp) return INVALID_PARAM;

    using OpenDDS::DCPS::Data_Types_Register;
    OpenDDS::DCPS::TypeSupport* ts =
      Registered_Data_Types->lookup(dp, type);
    if (!ts) {
      ts = Registered_Data_Types->lookup(0, type);
      if (!ts) return INVALID_PARAM;
      Registered_Data_Types->register_type(dp, type, ts);
    }

    const ::DDS::Topic_var topic =
      dp->create_topic(topicName, type, TOPIC_QOS_DEFAULT, 0, 0);
    if (!topic) return INVALID_PARAM;

    if (dir == SOURCE) {
      DDS::PublisherQos publisher_qos;
      qos_settings.apply_to(publisher_qos);
      
      const ::DDS::Publisher_var pub =
        dp->create_publisher(publisher_qos, 0, 0);
      if (!pub) return INVALID_PARAM;

      DDS::DataWriterQos datawriter_qos;
      qos_settings.apply_to(datawriter_qos);

      const ::DDS::DataWriter_var dw =
        pub->create_datawriter(topic, datawriter_qos, 0, 0);
      if (!dw) return INVALID_PARAM;

      Entities::instance()->writers_[connectionId] = dw;

    } else { // dir == DESTINATION
      DDS::SubscriberQos subscriber_qos;
      qos_settings.apply_to(subscriber_qos);
      
      const ::DDS::Subscriber_var sub =
        dp->create_subscriber(subscriber_qos, 0, 0);
      if (!sub) return INVALID_PARAM;

      DDS::DataReaderQos datareader_qos;
      qos_settings.apply_to(datareader_qos);

      const ::DDS::DataReader_var dr =
        sub->create_datareader(topic, datareader_qos, 0, 0);
      if (!dr) return INVALID_PARAM;

      Entities::instance()->readers_[connectionId] = dr;
    }

    return NO_ERROR;
  }
}

}}

namespace OpenDDS {
namespace FaceTSS {

Entities::Entities() {}
Entities::~Entities() {}

Entities* Entities::instance()
{
  return ACE_Singleton<Entities, ACE_Thread_Mutex>::instance();
}

enum { NSEC_PER_SEC = 1000000000 };

DDS::Duration_t convertTimeout(FACE::TIMEOUT_TYPE timeout)
{
  if (timeout == FACE::INF_TIME_VALUE) {
    static const DDS::Duration_t dds_inf = {DDS::DURATION_INFINITE_SEC,
                                            DDS::DURATION_INFINITE_NSEC};
    return dds_inf;
  }

  DDS::Duration_t dur = {static_cast<int>(timeout / NSEC_PER_SEC),
                         static_cast<unsigned int>(timeout % NSEC_PER_SEC)};
  return dur;
}


}}
