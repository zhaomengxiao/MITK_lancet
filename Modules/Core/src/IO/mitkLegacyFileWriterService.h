/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef LegacyFileWriterService_H_HEADER_INCLUDED_C1E7E521
#define LegacyFileWriterService_H_HEADER_INCLUDED_C1E7E521

#include <mitkAbstractFileWriter.h>
#include <mitkFileWriter.h>

namespace mitk
{
  class LegacyFileWriterService : public mitk::AbstractFileWriter
  {
  public:
    LegacyFileWriterService(mitk::FileWriter::Pointer legacyWriter, const std::string &description);

    ~LegacyFileWriterService() override;

    using AbstractFileWriter::Write;

    void Write() override;

    ConfidenceLevel GetConfidenceLevel() const override;

  private:
    LegacyFileWriterService *Clone() const override;

    mitk::FileWriter::Pointer m_LegacyWriter;

    us::ServiceRegistration<IFileWriter> m_ServiceRegistration;
  };

} // namespace mitk

#endif /* LegacyFileWriterService_H_HEADER_INCLUDED_C1E7E521 */
