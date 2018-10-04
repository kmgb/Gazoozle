#include "autostrafe.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"

void AutoStrafe::OnCreateMove(CUserCmd * cmd)
{
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		return;

	// TODO: Check for noclip and other things

	// Don't interfere with manual moves
	if (cmd->buttons & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT))
		return;

	auto hvel = g_LocalPlayer->m_vecVelocity();
	hvel.z = 0.f;

	QAngle velAngs;
	Math::VectorAngles(hvel, velAngs);

	QAngle lookAngs;
	g_EngineClient->GetViewAngles(lookAngs);

	float speed = hvel.Length2D();
	float maxspeed = g_LocalPlayer->m_flMaxspeed();
	//float maxspeed = g_LocalPlayer->m_flMaxspeed();

	/*float accelYaw;
	if (speed < 30.f)
	{
		// This might not be the fastest ever
		accelYaw = 0.f;
	}
	else
	{
		//float yawDelta = RAD2DEG(acos(30.f / speed));
		float yawDelta = acos(-27.32f / (speed*2));

		QAngle velViewDiffAng = velAngs - lookAngs;
		Math::NormalizeAngles(velViewDiffAng);

		accelYaw = velAngs.yaw;
		// Strafe left or right depending on where we need to rotate velocity
		accelYaw += (velViewDiffAng.yaw > 0.f) ? -yawDelta : yawDelta;

		accelYaw -= cmd->viewangles.yaw; // Make relative to viewangles

		QAngle accelAngs = { 0, accelYaw, 0 };
		Math::NormalizeAngles(accelAngs);
		accelYaw = accelAngs.yaw;
	}

	cmd->forwardmove = 450.f * cos(DEG2RAD(accelYaw));
	// sidemove left = negative/??/
	cmd->sidemove = -450.f * sin(DEG2RAD(accelYaw));
	*/


	// Accelerate forward unless we should do fancy strafes
	float accelYaw = lookAngs.yaw;

	if (speed > 0.f)
	{
		float delta{};

		QAngle velViewDiffAng = velAngs - lookAngs;
		Math::NormalizeAngles(velViewDiffAng);
		float velViewDiff = velViewDiffAng.yaw;

		if (velViewDiff > 30.f)
		{
			// For max turning:
			// theta = acos(-a / speed*2)
			delta = RAD2DEG(acos(-27.32f / (speed * 2)));
		}
		else
		{
			// For max speed:
			// theta = acos(30 / (airaccel * interval * wishspeed * speed))
			constexpr float airaccel = 12.f;
			float term = airaccel * g_GlobalVars->interval_per_tick * maxspeed * speed;

			if (term != 0.f)
			{
				term = 30.f / term;
				if (std::abs(term) < 1.f)
				{
					delta = RAD2DEG(acos(term));
				}
			}
		}

		accelYaw = velAngs.yaw;
		// Strafe left or right depending on where we need to rotate velocity
		accelYaw += (velViewDiffAng.yaw > 0.f) ? -delta : delta;

		//accelYaw -= cmd->viewangles.yaw; // Make relative to viewangles

		QAngle accelAngs = { 0, accelYaw, 0 };
		Math::NormalizeAngles(accelAngs);
		accelYaw = accelAngs.yaw;
	}

	cmd->viewangles.yaw = accelYaw;
	cmd->forwardmove = 450.f;
	cmd->sidemove = 0.f;
}
