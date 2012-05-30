/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * mt-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "mt/ThreadGroup.h"

mt::ThreadGroup::ThreadGroup()
{}

mt::ThreadGroup::~ThreadGroup()
{
    try
    {
        joinAll();
    }
    catch (...)
    {
        // Make sure we don't throw out of the destructor.
    }
}

void mt::ThreadGroup::createThread(sys::Runnable *runnable)
{
    createThread(std::auto_ptr<sys::Runnable>(runnable));
}

void mt::ThreadGroup::createThread(std::auto_ptr<sys::Runnable> runnable)
{
    sys::Thread *thread = new sys::Thread(runnable.get());
    threads.push_back(mem::SharedPtr<sys::Thread>(thread));
    thread->start();
	runnable.release();
}

void mt::ThreadGroup::joinAll()
{
    for (int i = 0; i < threads.size(); i++)
    {
        threads[i]->join();
    }
    threads.clear();
}
