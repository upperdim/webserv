/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBufferReader.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:07:47 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/15 20:22:41 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEBUFFERREADER_HPP
#define FILEBUFFERREADER_HPP

#include <string>
#include <fstream>
#include "Log.hpp"

namespace FileBuffer
{
	enum class State
	{
		UNINITIALIZED,
		INIT,
		READING,
		COMPLETE,
		ERROR
	};
}

class FileBufferReader
{
public:
	FileBufferReader();
	FileBufferReader(const FileBufferReader& other);
	FileBufferReader(const std::string& _path, const size_t& _buff_size);
	~FileBufferReader();

	FileBufferReader&	operator=(const FileBufferReader& rhs);
	std::string			getNextChunk(void);
	FileBuffer::State	getState(void);
	size_t				getSize(void);

private:

	void	open_ifs(const std::string& _path);
	void	close_ifs(void);

	FileBuffer::State	m_state;
	std::string			m_path;
	size_t				m_buff_size;
	std::ifstream		m_ifs;
	size_t				m_file_size;
};

#endif
