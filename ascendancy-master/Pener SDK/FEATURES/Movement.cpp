#include "../includes.h"
#include "../UTILS/mopvar.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Movement.h"
void CMovement::bunnyhop(SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (cmd->buttons & IN_JUMP)
	{
		int local_flags = local_player->GetFlags();
		if (!(local_flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;

		if (local_player->GetVelocity().Length() <= 50)
			cmd->forwardmove = 450.f;
	}
}

void CMovement::autostrafer(SDK::CUserCmd* cmd) //thanks toast
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	int local_flags = local_player->GetFlags();
	if (!(local_player->GetFlags() & local_flags & FL_ONGROUND))
	{
		if (cmd->mousedx > 1 || cmd->mousedx < -1)
			cmd->sidemove = cmd->mousedx < 0.0f ? -450 : 450;
		else 
		{
			cmd->forwardmove = 4200.0f / local_player->GetVelocity().Length();
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -450 : 450;
			if (cmd->forwardmove >  450)
				cmd->forwardmove = 450;
		}
	}
	else if (cmd->buttons & IN_JUMP)
		cmd->forwardmove = 450;
}

void CMovement::quick_stop(SDK::CBaseEntity* entity, SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer()); //initialize local player

	if (!local_player) //make sure this nigga aint null
		return;

	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset(); //get eye position

	if (entity->GetImmunity()) //make sure u dont stop when they in spawn protect
		return;

	if (entity->GetIsDormant()) //fuck dormant niggas
		return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(mopvar::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex())); //initialize weapon

	if (!weapon) //make sure u aint holdin aiR niGGa
		return;

	if (weapon->is_knife() || weapon->is_grenade() || weapon->GetItemDefenitionIndex() == SDK::WEAPON_TASER) //we dont wanna stop if we holdin a knife, grenade or zeus
		return;

	if (!aimbot->can_shoot()) //so it doesn't just fully freeze us while reloading or cocking
		return;

	if (autowall->CalculateDamage(local_position, aimbot->full_point(entity), local_player, entity).damage > SETTINGS::settings.damage_val) //autowall to the enemies position and see if they meet minimum damage
	{
		cmd->forwardmove = 450; //because fuck you
		aimbot->rotate_movement(UTILS::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f, cmd); //negate direction to fully stop
	}
}

CMovement* movement = new CMovement();