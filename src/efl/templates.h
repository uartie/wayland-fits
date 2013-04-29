/*
 * Copyright © 2013 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __WAYLAND_EFL_TEMPLATES_H__
#define __WAYLAND_EFL_TEMPLATES_H__

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <vector>

#include "elmtestharness.h"
#include "evasobject.h"
#include "window.h"

template <typename T>
class ResizeObjectTest : public ElmTestHarness
{
	BOOST_STATIC_ASSERT( (boost::is_base_of<EvasObject, T>::value) );

public:
	ResizeObjectTest()
		: ElmTestHarness::ElmTestHarness()
		, window_("ResizeObjectTest", "Resize Test")
		, object_(window_)
	{
		widths_.push_back(300);
		widths_.push_back(200);
		widths_.push_back(101);

		heights_.push_back(200);
		heights_.push_back(100);
		heights_.push_back(51);
	}

	void setup()
	{
		window_.show();
		object_.show();

		foreach (const int w, widths_) {
			foreach (const int h, heights_) {
				queueStep(boost::bind(&T::setSize, boost::ref(object_), w, h));
				queueStep(boost::bind(&T::checkSize, boost::ref(object_), w, h));
			}
		}
	}

private:
	Window			window_;
	T			object_;
	std::vector<int>	widths_;
	std::vector<int>	heights_;
};

template <typename T>
class PositionObjectTest : public ElmTestHarness
{
	BOOST_STATIC_ASSERT( (boost::is_base_of<EvasObject, T>::value) );

public:
	PositionObjectTest()
		: ElmTestHarness::ElmTestHarness()
		, window_("PositionObjectTest", "Position Test")
		, object_(window_)
	{
		xs_.push_back(50);
		xs_.push_back(100);
		xs_.push_back(151);

		ys_.push_back(10);
		ys_.push_back(50);
		ys_.push_back(101);

		object_.setSize(50, 50);
	}

	void setup()
	{
		window_.show();
		object_.show();

		foreach (const int x, xs_) {
			foreach (const int y, ys_) {
				queueStep(boost::bind(&T::setPosition, boost::ref(object_), x, y));
				queueStep(boost::bind(&T::checkPosition, boost::ref(object_), x, y));
			}
		}
	}

private:
	Window			window_;
	T			object_;
	std::vector<int>	xs_;
	std::vector<int>	ys_;
};


template <typename T>
class VisibleObjectTest : public ElmTestHarness
{
	BOOST_STATIC_ASSERT( (boost::is_base_of<EvasObject, T>::value) );

public:
	VisibleObjectTest()
		: ElmTestHarness::ElmTestHarness()
		, window_("VisibleObjectTest", "Visibility Test")
		, object_(window_)
	{
		object_.setSize(50, 50);
		object_.setPosition(100, 100);

		return;
	}

	void setup()
	{
		window_.show();

		queueStep(boost::bind(&T::show, boost::ref(object_)));
		queueStep(boost::bind(&T::checkVisible, boost::ref(object_), EINA_TRUE));

		queueStep(boost::bind(&T::hide, boost::ref(object_)));
		queueStep(boost::bind(&T::checkVisible, boost::ref(object_), EINA_FALSE));

		queueStep(boost::bind(&T::show, boost::ref(object_)));
		queueStep(boost::bind(&T::checkVisible, boost::ref(object_), EINA_TRUE));
	}

private:
	Window	window_;
	T	object_;
};

/* TODO: evasobject methods for color set/get/check
template <typename T, int Tr1, int Tg1, int Tb1, int Ta1, int Tr2, int Tg2, int Tb2, int Ta2>
class ColorObjectTest : public ElmTestHarness
{
	BOOST_STATIC_ASSERT( (boost::is_base_of<EvasObject, T>::value) );

public:
	ColorObjectTest()
			: ElmTestHarness::ElmTestHarness()
			, window_("ColorObjectTest", "Color Test")
			, object_(window_)
	{
		return;
	}

	void setup()
	{
		queueCallback(
			ModifyCheckCallback(
				boost::bind(&T::setColor, boost::ref(object_), Tr1, Tg1, Tb1, Ta1),
				boost::bind(&T::checkColor, boost::ref(object_), Tr1, Tg1, Tb1, Ta1)
			)
		);

		queueCallback(
			ModifyCheckCallback(
				boost::bind(&T::setColor, boost::ref(object_), Tr2, Tg2, Tb2, Ta2),
				boost::bind(&T::checkColor, boost::ref(object_), Tr2, Tg2, Tb2, Ta2)
			)
		);

		queueCallback(
			ModifyCheckCallback(
				boost::bind(&T::setColor, boost::ref(object_), Tr1, Tg1, Tb1, Ta1),
				boost::bind(&T::checkColor, boost::ref(object_), Tr1, Tg1, Tb1, Ta1)
			)
		);
	}

private:
	Window	window_;
	T	object_;
};
*/

/* TODO: evasobject methods for raise/lower set/get/check
template <typename T>
class RaiseLowerObjectTest : public ElmTestHarness
{
	BOOST_STATIC_ASSERT( (boost::is_base_of<EvasObject, T>::value) );

public:
	RaiseLowerObjectTest()
			: ElmTestHarness::ElmTestHarness()
			, window_("RaiseLowerObjectTest", "RaiseLower Test")
			, object_(window_)
	{
		return;
	}

	void setup()
	{
		queueCallback(
			ModifyCheckCallback(
				boost::bind(&T::raise, boost::ref(object_)),
				boost::bind(&T::checkRisen, boost::ref(object_), EINA_TRUE)
			)
		);

		queueCallback(
			ModifyCheckCallback(
				boost::bind(&T::lower, boost::ref(object_)),
				boost::bind(&T::checkLowered, boost::ref(object_), EINA_TRUE)
			)
		);

		queueCallback(
			ModifyCheckCallback(
				boost::bind(&T::raise, boost::ref(object_)),
				boost::bind(&T::checkRisen, boost::ref(object_), EINA_TRUE)
			)
		);
	}

private:
	Window	window_;
	T	object_;
};
*/

#endif

