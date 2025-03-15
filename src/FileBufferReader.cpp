/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBufferReader.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:07:45 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/15 20:26:10 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileBufferReader.hpp"

FileBufferReader::FileBufferReader()
	:	m_state(FileBuffer::State::UNINITIALIZED),
		m_path(""),
		m_buff_size(0),
		m_ifs(0),
		m_file_size(0)
{
}

FileBufferReader::FileBufferReader(const FileBufferReader& other)
	:	m_state(other.m_state),
		m_path(other.m_path),
		m_buff_size(other.m_buff_size),
		m_ifs(0),
		m_file_size(0)
{
	open_ifs(m_path);
}

FileBufferReader::FileBufferReader(const std::string& _path, const size_t& _buff_size)
	:	m_state(FileBuffer::State::INIT),
		m_path(_path),
		m_buff_size(_buff_size),
		m_ifs(0),
		m_file_size(0)
{
	open_ifs(_path);
	m_state = FileBuffer::State::READING;
}

FileBufferReader::~FileBufferReader()
{
	close_ifs();
}


/* ************************************************************************** */
/* ************************************************************************** */


FileBufferReader&	FileBufferReader::operator=(const FileBufferReader& rhs)
{
	if (this != &rhs)
	{
		close_ifs();
		m_state = rhs.m_state;
		m_path = rhs.m_path;
		m_buff_size = rhs.m_buff_size;
		open_ifs(m_path);
	}
	return (*this);
}

std::string	FileBufferReader::getNextChunk(void)
{
	std::string	content(m_buff_size, '\0');
	m_ifs.read(&content[0], m_buff_size);
	
	content.resize(m_ifs.gcount());

	if (m_ifs.eof())
		m_state = FileBuffer::State::COMPLETE;

	return (content);
}

FileBuffer::State	FileBufferReader::getState(void)
{
	return (m_state);
}

size_t	FileBufferReader::getSize(void)
{
	return (m_file_size);
}


/* ************************************************************************** */
/* ************************************************************************** */

void	FileBufferReader::open_ifs(const std::string& _path)
{
	m_ifs = std::ifstream(m_path, std::ios::binary);

	m_ifs.open(_path);
	if (!m_ifs.is_open())
	{
		m_state = FileBuffer::State::ERROR;
		return ;
	}

	// get the file size
    m_ifs.seekg (0, m_ifs.end);
    m_file_size = m_ifs.tellg();
    m_ifs.seekg (0, m_ifs.beg);
}

void	FileBufferReader::close_ifs(void)
{
	if (m_ifs.is_open())
	{
		LOG_WARNING_LM("CLOSING", std::string("FileBufferReader ") + m_path);
		m_ifs.close();
	}
}
