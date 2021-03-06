/***
    This file is part of snapcast
    Copyright (C) 2014-2020  Johannes Pohl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef PROCESS_STREAM_HPP
#define PROCESS_STREAM_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#include <boost/process.hpp>
#pragma GCC diagnostic pop
#include <memory>
#include <string>
#include <vector>

#include "posix_stream.hpp"
#include "watchdog.hpp"


namespace bp = boost::process;


namespace streamreader
{

/// Starts an external process and reads and PCM data from stdout
/**
 * Starts an external process, reads PCM data from stdout, and passes the data to an encoder.
 * Implements EncoderListener to get the encoded data.
 * Data is passed to the PcmListener
 */
class ProcessStream : public PosixStream, public WatchdogListener
{
public:
    /// ctor. Encoded PCM data is passed to the PipeListener
    ProcessStream(PcmListener* pcmListener, boost::asio::io_context& ioc, const StreamUri& uri);
    ~ProcessStream() override = default;

protected:
    void do_connect() override;
    void do_disconnect() override;

    std::string exe_;
    std::string path_;
    std::string params_;
    bp::pipe pipe_stdout_;
    bp::pipe pipe_stderr_;
    bp::child process_;

    bool logStderr_;
    boost::asio::streambuf streambuf_stderr_;
    std::unique_ptr<stream_descriptor> stream_stderr_;

    // void worker() override;
    virtual void stderrReadLine();
    virtual void onStderrMsg(const std::string& line);
    virtual void initExeAndPath(const std::string& filename);

    bool fileExists(const std::string& filename) const;
    std::string findExe(const std::string& filename) const;

    size_t wd_timeout_sec_;
    std::unique_ptr<Watchdog> watchdog_;
    void onTimeout(const Watchdog& watchdog, std::chrono::milliseconds ms) override;
};

} // namespace streamreader

#endif
