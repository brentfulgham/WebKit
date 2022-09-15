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

#include "config.h"
#include "DeprecationReportBody.h"

#include "DateComponents.h"
#include "FormData.h"
#include <wtf/IsoMallocInlines.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(DeprecationReportBody);

const AtomString& DeprecationReportBody::deprecationReportType()
{
    static NeverDestroyed<AtomString> reportType { "deprecation"_s };
    return reportType;
}

DeprecationReportBody::DeprecationReportBody(String&& id, WallTime&& anticipatedRemoval, String&& message, String&& sourceFile)
    : ReportBody(ViolationReportType::Deprecation)
    , m_id(WTFMove(id))
    , m_anticipatedRemoval(WTFMove(anticipatedRemoval))
    , m_message(WTFMove(message))
    , m_sourceFile(WTFMove(sourceFile))
{
}

Ref<DeprecationReportBody> DeprecationReportBody::create(String&& id, WallTime&& anticipatedRemoval, String&& message, String&& sourceFile)
{
    return adoptRef(*new DeprecationReportBody(WTFMove(id), WTFMove(anticipatedRemoval), WTFMove(message), WTFMove(sourceFile)));
}

const AtomString& DeprecationReportBody::type() const
{
    return deprecationReportType();
}

const String& DeprecationReportBody::id() const
{
    return m_id;
}

WallTime DeprecationReportBody::anticipatedRemoval() const
{
    return m_anticipatedRemoval;
}

const String& DeprecationReportBody::message() const
{
    return m_message;
}

const String& DeprecationReportBody::sourceFile() const
{
    return m_sourceFile;
}

std::optional<unsigned> DeprecationReportBody::lineNumber() const
{
    return m_lineNumber;
}

std::optional<unsigned> DeprecationReportBody::columnNumber() const
{
    return m_columnNumber;
}

Ref<FormData> DeprecationReportBody::createReportFormDataForViolation() const
{
    // https://wicg.github.io/deprecation-reporting/#deprecation-report
    // Suitable for network endpoints.
    auto reportBody = JSON::Object::create();
    reportBody->setString("id"_s, m_id);
    reportBody->setString("anticipatedRemoval"_s, DateComponents::fromMillisecondsSinceEpochForDate(m_anticipatedRemoval.secondsSinceEpoch().milliseconds())->toString());
    reportBody->setString("message"_s, m_message);
    if (!m_sourceFile.isNull()) {
        reportBody->setString("sourceFile"_s, m_sourceFile);
        reportBody->setInteger("lineNumber"_s, m_lineNumber ? *m_lineNumber : 0);
        reportBody->setInteger("columnNumber"_s, m_columnNumber ? *m_columnNumber : 0);
    }

    auto reportObject = JSON::Object::create();
    reportObject->setString("type"_s, deprecationReportType());
    reportObject->setString("url"_s, ""_s);
    reportObject->setObject("body"_s, WTFMove(reportBody));

    return FormData::create(reportObject->toJSONString().utf8());
}

} // namespace WebCore
