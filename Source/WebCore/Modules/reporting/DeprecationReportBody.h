/*
 * Copyright (C) 2022 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "ReportBody.h"
#include "ViolationReportType.h"
#include <wtf/IsoMalloc.h>
#include <wtf/WallTime.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class WEBCORE_EXPORT DeprecationReportBody final : public ReportBody {
    WTF_MAKE_ISO_ALLOCATED(DeprecationReportBody);
public:
    static Ref<DeprecationReportBody> create(String&& id, WallTime&& anticipatedRemoval, String&& message, String&& sourceFile);

    const String& id() const;
    WallTime anticipatedRemoval() const;
    const String& message() const;
    const String& sourceFile() const;
    std::optional<unsigned> lineNumber() const;
    std::optional<unsigned> columnNumber() const;

    static const AtomString& deprecationReportType();

    Ref<FormData> createReportFormDataForViolation() const;

    template<typename Encoder> void encode(Encoder&) const;
    template<typename Decoder> static std::optional<RefPtr<WebCore::DeprecationReportBody>> decode(Decoder&);

private:
    DeprecationReportBody(String&& id, WallTime&& anticipatedRemoval, String&& message, String&& sourceFile);

    const AtomString& type() const final;

    const String m_id;
    WallTime m_anticipatedRemoval;
    const String m_message;
    const String m_sourceFile;
    std::optional<unsigned> m_lineNumber;
    std::optional<unsigned> m_columnNumber;
};

template<typename Encoder>
void DeprecationReportBody::encode(Encoder& encoder) const
{
    encoder << m_id << m_anticipatedRemoval << m_message << m_sourceFile;
}

template<typename Decoder>
std::optional<RefPtr<DeprecationReportBody>> DeprecationReportBody::decode(Decoder& decoder)
{
    std::optional<String> reportId;
    decoder >> reportId;
    if (!reportId)
        return std::nullopt;

    std::optional<WallTime> anticipatedRemoval;
    decoder >> anticipatedRemoval;
    if (!anticipatedRemoval)
        return std::nullopt;

    std::optional<String> message;
    decoder >> message;
    if (!message)
        return std::nullopt;

    std::optional<String> sourceFile;
    decoder >> sourceFile;
    if (!sourceFile)
        return std::nullopt;

    return adoptRef(new DeprecationReportBody(WTFMove(*reportId), WTFMove(*anticipatedRemoval), WTFMove(*message), WTFMove(*sourceFile)));
}

} // namespace WebCore

SPECIALIZE_TYPE_TRAITS_BEGIN(WebCore::DeprecationReportBody)
    static bool isType(const WebCore::ReportBody& reportBody) { return reportBody.reportBodyType() == WebCore::ViolationReportType::Deprecation; }
SPECIALIZE_TYPE_TRAITS_END()
