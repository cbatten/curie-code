//========================================================================
// CurieCloud
//========================================================================

#ifndef CURIE_CLOUD_H
#define CURIE_CLOUD_H

#include <Arduino.h>

//------------------------------------------------------------------------
// Xively Feed ID and API Keys
//------------------------------------------------------------------------
// If CURIE_CLOUD_GROUP_NUM is defined we use that to choose the
// appropriate feed ID and API key, otherwise we assume the user has
// already defined CURIE_CLOUD_FEED_ID and CURIE_CLOUD_API_KEY. Note that
// group number 0 is the CURIE Test device.

#define CURIE_CLOUD_GROUP( num_ )                                       \
  F(CURIE_CLOUD_FEED_ID_ ## num_),                                      \
  F(CURIE_CLOUD_API_KEY_ ## num_)                                       \

#define CURIE_CLOUD_FEED_ID_0 "324666390"
#define CURIE_CLOUD_API_KEY_0 "Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk"

// #define CURIE_CLOUD_FEED_ID_1 "1109083764"
// #define CURIE_CLOUD_API_KEY_1 "OsgVS5ORksByelZC8eLpgCrpzoBHNadpoEwKxBM0m0nnWsPF"

#define CURIE_CLOUD_FEED_ID_1 "786347404"
#define CURIE_CLOUD_API_KEY_1 "NdUJGIgjBBnAK9nKqoVjQwrUqls8hJNEGtpzwD8RHSZOyJ62"

// #define CURIE_CLOUD_FEED_ID_2 "128515174"
// #define CURIE_CLOUD_API_KEY_2 "kF7iu8UJRfCDdIdFsye9wQvAhEcxnUN3SNzV6gvQgaVhkk3Z"

#define CURIE_CLOUD_FEED_ID_2 "1005542916"
#define CURIE_CLOUD_API_KEY_2 "GmlkOMaUoZnCHeiMpIhziwQvJJhOm22hp1K2BIaiihrnADAZ"

// #define CURIE_CLOUD_FEED_ID_3 "1104740964"
// #define CURIE_CLOUD_API_KEY_3 "375vDQ6z6mOljn0Kfh6BDRI24bgnKlfRKQE7gtbsdRUmPk6X"

#define CURIE_CLOUD_FEED_ID_3 "1777797459"
#define CURIE_CLOUD_API_KEY_3 "3LbbTxYZlFAb3kiXfDN1Wpmd8YC6rG4kUuzDT2cyD5ZsH4B2"

// #define CURIE_CLOUD_FEED_ID_4 "1274492388"
// #define CURIE_CLOUD_API_KEY_4 "ZW6rAjcbI1u7NOxpXWfZlC7GmkwHogybq1yRPzRYB2ASvTBf"

#define CURIE_CLOUD_FEED_ID_4 "1561668635"
#define CURIE_CLOUD_API_KEY_4 "zl8Fk9ER5oKSgWpDZ0Ah519TPM89fgSYiLvu7K0zASBcvabv"

// #define CURIE_CLOUD_FEED_ID_5 "204128726"
// #define CURIE_CLOUD_API_KEY_5 "hrGEBDbU4YedkMzst4kqQTQLeAaIsAP0HnIU40n7R7WBfBip"

#define CURIE_CLOUD_FEED_ID_5 "172009280"
#define CURIE_CLOUD_API_KEY_5 "BMS6Uu8dX1VCBGjCWKF79DEWl6GpqHyAg8QCtPn6GznZB4rt"

// #define CURIE_CLOUD_FEED_ID_6 "332106916"
// #define CURIE_CLOUD_API_KEY_6 "5wRRJMSuQJ45QOymG4dO1sIXPkAMtSXEi5ryCDoIQCM2rNsB"

#define CURIE_CLOUD_FEED_ID_6 "677281936"
#define CURIE_CLOUD_API_KEY_6 "w4FGj8G4n9XFhArZZdKUaSybbOcacaxQbVHYDoomWRRq7gUI"

// #define CURIE_CLOUD_FEED_ID_7 "911427940"
// #define CURIE_CLOUD_API_KEY_7 "Pjtcoh9FdQ2mQ1O9wggkPZkzPPE8OuQmo0FWTNLCqL0oZVRQ"

#define CURIE_CLOUD_FEED_ID_7 "147400025"
#define CURIE_CLOUD_API_KEY_7 "lgMCu78jsBkpWMQQL9MT3exILPFbRBYL6KwzFNZkYC6UwBfy"

// #define CURIE_CLOUD_FEED_ID_8 "435629229"
// #define CURIE_CLOUD_API_KEY_8 "qDUG94RvFPznlgemORbSwxvJeo7YwruRB2dpWArGHM6Jb442"

#define CURIE_CLOUD_FEED_ID_8 "340659114"
#define CURIE_CLOUD_API_KEY_8 "xfXbjMG5pYbguWj9iyJr05B1SVy7LSvYETgsAvUUp0HQIyAw"

class CurieCloud
{
 public:

  CurieCloud();

  // Setup curie cloud (setup wifi board, lcd, connect to red rover)

  void begin( const __FlashStringHelper* feed_id,
              const __FlashStringHelper* api_key );

  // Send data to a Xively channel

  void send_int( const char* name, int value );
  void send_float( const char* name, float value );
  void send_str( const char* name, const String& value );

  // Receive data from a Xively channel

  int recv_int( const char* name );
  float  recv_float( const char* name );
  String recv_str( const char* name );

 private:

  const __FlashStringHelper* m_feed_id;
  const __FlashStringHelper* m_api_key;

  bool m_initialized;

};

// Global instance

extern CurieCloud curie_cloud;

#endif /* CURIE_CLOUD_H */

