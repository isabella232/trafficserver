/** @file
 *
 *  A brief file description
 *
 *  @section license License
 *
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <deque>
#include "QUICFrame.h"

class QUICFrameGenerator;

struct QUICFrameInformation {
  QUICFrameType type;
  QUICEncryptionLevel level;

  uint8_t data[1024] = {};
};

typedef std::unique_ptr<QUICFrameInformation> QUICFrameInformationUPtr;

constexpr QUICFrameType RETRANSMITTED_FRAME_TYPE[] = {QUICFrameType::STREAM, QUICFrameType::STOP_SENDING,
                                                      QUICFrameType::MAX_STREAM_DATA, QUICFrameType::RST_STREAM,
                                                      QUICFrameType::MAX_DATA};

struct StreamFrameInfo {
  QUICStreamId stream_id;
  QUICOffset offset;
  bool has_fin;
  Ptr<IOBufferBlock> block;
};

struct MaxStreamDataFrameInfo {
  QUICStreamId stream_id;
  uint64_t maximum_stream_data;
};

struct RstStreamFrameInfo {
  QUICStreamId stream_id;
  QUICAppErrorCode error_code;
  QUICOffset final_offset;
};

struct StopSendingFrameInfo {
  QUICStreamId stream_id;
  QUICAppErrorCode error_code;
};

struct MaxDataFrameInfo {
  uint64_t maximum_data;
};

struct BlockedFrameInfo {
  QUICOffset offset;
};

struct StreamBlockedFrameInfo {
  QUICStreamId stream_id;
  QUICOffset offset;
};

struct CryptoFrameInfo {
  QUICOffset offset;
  Ptr<IOBufferBlock> block;
};

struct AckFrameInfo {
  QUICPacketNumber largest_acknowledged;
};

class QUICFrameRetransmitter
{
public:
  virtual QUICFrameUPtr create_retransmitted_frame(QUICEncryptionLevel level, uint16_t maximum_frame_size, QUICFrameId id = 0,
                                                   QUICFrameGenerator *owner = nullptr);
  virtual void save_frame_info(QUICFrameInformationUPtr &info);

private:
  QUICFrameUPtr _create_stream_frame(QUICFrameInformationUPtr &info, uint16_t maximum_frame_size,
                                     std::deque<QUICFrameInformationUPtr> &tmp_queue, QUICFrameId id, QUICFrameGenerator *owner);
  QUICFrameUPtr _create_reset_stream_frame(QUICFrameInformationUPtr &info, uint16_t maximum_frame_size,
                                           std::deque<QUICFrameInformationUPtr> &tmp_queue, QUICFrameId id,
                                           QUICFrameGenerator *owner);
  QUICFrameUPtr _create_max_stream_data_frame(QUICFrameInformationUPtr &info, uint16_t maximum_frame_size,
                                              std::deque<QUICFrameInformationUPtr> &tmp_queue, QUICFrameId id,
                                              QUICFrameGenerator *owner);
  QUICFrameUPtr _create_stop_sending_frame(QUICFrameInformationUPtr &info, uint16_t maximum_frame_size,
                                           std::deque<QUICFrameInformationUPtr> &tmp_queue, QUICFrameId id,
                                           QUICFrameGenerator *owner);
  QUICFrameUPtr _create_max_data_frame(QUICFrameInformationUPtr &info, uint16_t maximum_frame_size,
                                       std::deque<QUICFrameInformationUPtr> &tmp_queue, QUICFrameId id, QUICFrameGenerator *owner);

  void _append_info_queue(std::deque<QUICFrameInformationUPtr> &tmp_queue);

  std::deque<QUICFrameInformationUPtr> _lost_frame_info_queue;
};