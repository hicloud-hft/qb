/*
 * qb - C++ Actor Framework
 * Copyright (C) 2011-2019 isndev (www.qbaf.io). All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *         limitations under the License.
 */

#ifndef             QB_IO_PROT_BASE_H_
# define            QB_IO_PROT_BASE_H_
#include "../stream.h"

namespace qb {
    namespace io {
        namespace protocol {

            template<typename _Stream, char _SEP = '\n'>
            class Base : public _Stream {
            public:
                using message_type = const char *;

                int getMessageSize() {
                    auto i = this->_in_buffer.begin();
                    while (i < this->_in_buffer.end()) {
                        if (this->_in_buffer.data()[i] == _SEP)
                            return i - this->_in_buffer.begin() + 1;
                        ++i;
                    }
                    return 0;
                }

                message_type getMessage() {
                    return this->_in_buffer.data() + this->_in_buffer.begin();
                }
            };

        } // namespace protocol
    } // namespace io
} // namespace qb

#endif // QB_IO_PROT_BASE_H_