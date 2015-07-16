//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef __REQUESTHANDLER_H
#define __REQUESTHANDLER_H

#include <ResourceObject.h>
#include <PrimitiveResponse.h>

#include <internal/ResourceAttributesUtils.h>

namespace OC
{
    class OCResourceResponse;
    class OCRepresentation;
}

namespace OIC
{
    namespace Service
    {
        class RequestHandler
        {
        private:
            typedef std::function< std::shared_ptr< OC::OCResourceResponse >(ResourceObject&) >
                        BuildResponseHolder;

        public:
            typedef std::shared_ptr< RequestHandler > Pre;

            static constexpr int DEFAULT_ERROR_CODE = 200;
            static constexpr OCEntityHandlerResult DEFAULT_RESULT = OC_EH_OK;

            RequestHandler();

            RequestHandler(const RequestHandler&) = delete;
            RequestHandler(RequestHandler&&) = default;

            RequestHandler(const OCEntityHandlerResult& result, int errorCode);

            RequestHandler(const ResourceAttributes&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);

            RequestHandler(ResourceAttributes&&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);


            virtual ~RequestHandler() { };

            std::shared_ptr< OC::OCResourceResponse > buildResponse(ResourceObject&);

        private:
            const BuildResponseHolder m_holder;
        };

        class SetRequestHandler: public RequestHandler
        {
        public:
            typedef std::shared_ptr< SetRequestHandler > Ptr;

            SetRequestHandler(const SetRequestHandler&) = delete;
            SetRequestHandler(SetRequestHandler&&) = default;

            SetRequestHandler();

            SetRequestHandler(const OCEntityHandlerResult& result, int errorCode);

            SetRequestHandler(const ResourceAttributes&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);

            SetRequestHandler(ResourceAttributes&&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);

            AttrKeyValuePairs applyAcceptanceMethod(PrimitiveSetResponse::AcceptanceMethod,
                    ResourceObject&, const ResourceAttributes&) const;
        };

    }
}

#endif // __REQUESTHANDLER_H
