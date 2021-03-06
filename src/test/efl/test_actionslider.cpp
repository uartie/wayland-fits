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

//---- Use EO API ----//
#define EFL_EO_API_SUPPORT // #undef is at end-of-file
#define EFL_BETA_API_SUPPORT // #undef is at end-of-file

//---- The following set of defines/includes make this work for 1.7.x ----//
#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <Evas.h>
#include <Elementary.h>
#include <elm_widget.h>
//-----------------------------------------------------------------------//

#include <elm_widget_actionslider.h>

#include <vector>
#include <linux/input.h>

#include "templates.h"
#include "config.h"

namespace wfits {
namespace test {
namespace efl {

using std::vector;

class Actionslider : public EvasObject
{
public:
	Actionslider(EvasObject &parent)
		: EvasObject::EvasObject(elm_actionslider_add(parent))
	{
		return;
	}

	Geometry getButtonGeometry() const
	{
		Evas_Object *o(NULL);

		Application::synchronized(
			[this, &o]() {
				/*
				 * NOTE: We can't use the ELM_ACTIONSLIDER_DATA_GET
				 * macro from elementary because it does not explicitly
				 * cast the underlying void* return value... our
				 * compiler flags will not allow that.
				 */
				#if EFL_VERSION_AT_LEAST(1, 10, 99) // version 1.11
				Elm_Actionslider_Data *data =
					static_cast<Elm_Actionslider_Data*>(
						eo_data_scope_get(*this, ELM_ACTIONSLIDER_CLASS)
				);
				#elif EFL_VERSION_AT_LEAST(1, 10, 0) // version 1.10
				Elm_Actionslider_Data *data =
					static_cast<Elm_Actionslider_Data*>(
						eo_data_scope_get(*this, ELM_OBJ_ACTIONSLIDER_CLASS)
				);
				#elif EFL_VERSION_AT_LEAST(1, 8, 0) // version 1.8 and 1.9
				Elm_Actionslider_Smart_Data *data =
					static_cast<Elm_Actionslider_Smart_Data*>(
						eo_data_scope_get(*this, ELM_OBJ_ACTIONSLIDER_CLASS)
					);
				#else // assume 1.7
				Elm_Actionslider_Smart_Data *data =
					static_cast<Elm_Actionslider_Smart_Data*>(
						evas_object_smart_data_get(*this)
					);
				#endif
				o = data->drag_button_base;
			}
		);
		ASSERT(o != NULL);
		return EvasObject(o, false).getGeometry();
	}
};

class ActionsliderIndicatorTest : public ElmTestHarness
{
public:
	ActionsliderIndicatorTest()
		: ElmTestHarness::ElmTestHarness()
		, window_("ActionsliderIndicatorTest", "Actionslider Position Test")
		, control_(window_)
	{
		positions_.push_back(ELM_ACTIONSLIDER_LEFT);
		positions_.push_back(ELM_ACTIONSLIDER_CENTER);
		positions_.push_back(ELM_ACTIONSLIDER_LEFT);
		positions_.push_back(ELM_ACTIONSLIDER_RIGHT);
		positions_.push_back(ELM_ACTIONSLIDER_CENTER);
		positions_.push_back(ELM_ACTIONSLIDER_RIGHT);
		positions_.push_back(ELM_ACTIONSLIDER_LEFT);
		positions_.push_back(ELM_ACTIONSLIDER_RIGHT);
		positions_.push_back(ELM_ACTIONSLIDER_CENTER);
	}

	void setup()
	{
		window_.show();
		control_.show();

		control_.setSize(200, 100);
		control_.setPosition(50, 10);
	}

	void test()
	{
		foreach (Elm_Actionslider_Pos p, positions_) {
			synchronized(boost::bind(elm_actionslider_indicator_pos_set, boost::ref(control_), p));
			synchronized(boost::bind(&ActionsliderIndicatorTest::checkPos, boost::ref(*this), p));
		}
	}

	void checkPos(const Elm_Actionslider_Pos expected)
	{
		FAIL_UNLESS_EQUAL(elm_actionslider_indicator_pos_get(control_), expected);
	}

private:
	Window				window_;
	Actionslider			control_;
	vector<Elm_Actionslider_Pos>	positions_;
};

class ActionSliderUserTest : public ElmTestHarness
{
public:
	ActionSliderUserTest()
		: ElmTestHarness::ElmTestHarness()
		, window_("ActionSliderUserTest", "ActionSlider User Test")
		, slider_(window_)
		, position_()
		, selection_()
		, rendered_(false)
	{
		return;
	}

	void setup()
	{
		evas_event_callback_add(evas_object_evas_get(window_), EVAS_CALLBACK_RENDER_POST, onPostRender, this);

		window_.show();

		int w(200), h(50);
		int x(window_.getWidth() / 2 - w / 2);
		int y(window_.getHeight() / 2 - h / 2);

		slider_.setSize(w, h);
		slider_.setPosition(x, y);

		elm_object_part_text_set(slider_, "left", "Left");
		elm_object_part_text_set(slider_, "center", "Center");
		elm_object_part_text_set(slider_, "right", "Right");

		evas_object_smart_callback_add(slider_, "selected", onSelect, this);
		evas_object_smart_callback_add(slider_, "pos_changed", onChange, this);

		slider_.show();
	}

	void test()
	{
		YIELD_UNTIL(rendered_);
		moveSliderTo(ELM_ACTIONSLIDER_CENTER);
		moveSliderTo(ELM_ACTIONSLIDER_RIGHT);
		moveSliderTo(ELM_ACTIONSLIDER_LEFT);
		moveSliderTo(ELM_ACTIONSLIDER_RIGHT);
		moveSliderTo(ELM_ACTIONSLIDER_CENTER);
		moveSliderTo(ELM_ACTIONSLIDER_LEFT);
	}

	void moveSliderTo(const Elm_Actionslider_Pos pos)
	{
		Geometry gw(getSurfaceGeometry(window_.get_wl_surface()));
		Geometry gf(window_.getFramespaceGeometry());
		Geometry gb(slider_.getButtonGeometry());
		Geometry gs(slider_.getGeometry());

		setGlobalPointerPosition(
			gw.x + gf.x + gb.x + gb.width / 2,
			gw.y + gf.y + gb.y + gb.height / 2
		);
		inputKeySend(BTN_LEFT, 1);

		std::string position, selection;
		int32_t x;

		switch (pos) {
			case ELM_ACTIONSLIDER_LEFT:
				position = "left";
				selection = "Left";
				x = gw.x + gf.x + gs.x + gb.width / 2;
				break;
			case ELM_ACTIONSLIDER_CENTER:
				position = "center";
				selection = "Center";
				x = gw.x + gf.x + gs.x + gs.width / 2;
				break;
			default:
				position = "right";
				selection = "Right";
				x = gw.x + gf.x + gs.x + gs.width - gb.width / 2;
				break;
		}

		setGlobalPointerPosition(x, gw.y + gf.y + gb.y + gb.height / 2);

		TEST_LOG("checking for position changed event on '" << position << "'");
		YIELD_UNTIL(position_ == position);

		inputKeySend(BTN_LEFT, 0);

		TEST_LOG("checking for selected event on '" << selection << "'");
		YIELD_UNTIL(selection_ == selection);
	}

	static void onPostRender(void *data, Evas *e, void *info)
	{
		evas_event_callback_del(e, EVAS_CALLBACK_RENDER_POST, onPostRender);

		ActionSliderUserTest *test = static_cast<ActionSliderUserTest*>(data);
		test->rendered_ = true;
		TEST_LOG("got post render event");
	}

	static void onSelect(void* data, Evas_Object*, void* info)
	{
		ActionSliderUserTest *test = static_cast<ActionSliderUserTest*>(data);
		test->selection_ = std::string(static_cast<char*>(info));
		TEST_LOG("received selected event on '" << test->selection_ << "'");
	}

	static void onChange(void* data, Evas_Object*, void* info)
	{
		ActionSliderUserTest *test = static_cast<ActionSliderUserTest*>(data);
		test->position_ = std::string(static_cast<char*>(info));;
		TEST_LOG("received position changed event on '" << test->position_ << "'");
	}

private:
	Window		window_;
	Actionslider	slider_;
	std::string	position_;
	std::string	selection_;
	bool		rendered_;
};

typedef ResizeObjectTest<Actionslider> ActionsliderResizeTest;
typedef PositionObjectTest<Actionslider> ActionsliderPositionTest;
typedef VisibleObjectTest<Actionslider> ActionsliderVisibilityTest;

WFITS_EFL_HARNESS_TEST_CASE(ActionsliderResizeTest)
WFITS_EFL_HARNESS_TEST_CASE(ActionsliderPositionTest)
WFITS_EFL_HARNESS_TEST_CASE(ActionsliderVisibilityTest)
WFITS_EFL_HARNESS_TEST_CASE(ActionsliderIndicatorTest)
WFITS_EFL_HARNESS_TEST_CASE(ActionSliderUserTest)

} // namespace efl
} // namespace test
} // namespace wfits

#undef EFL_EO_API_SUPPORT
#undef EFL_BETA_API_SUPPORT
