#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <aml-psdk/game_sa/base/Timer.h>
#include <aml-psdk/game_sa/controls.h> // Para sa control constants
#include <aml-psdk/game_sa/player.h>   // Para sa player functions
#include <aml-psdk/game_sa/vehicle.h>  // Para sa vehicle functions

MYMODCFG(net.psdk.mymod.guid, AML PSDK Template, 1.0, Author)

static int holdCounter = 0;
static bool isSkidding = false;
static bool tireFlat = false;
static int skidStartTime = 0;
static int skidDuration = 3000; // milliseconds before tire goes flat

// Function to get current vehicle player is in
int GetPlayerVehicle()
{
    int ped = PLAYER_PED_ID();
    int vehicle = GET_VEHICLE_PED_IS_IN(ped, false);
    return vehicle;
}

// Function to simulate fire on tire (placeholder)
void CreateFireOnTire(int vehicle, int tireIndex)
{
    logger->Info("Fire effect activated on tire %d of vehicle %d", tireIndex, vehicle);
}

// Function to set tire flat (placeholder)
void SetTireFlat(int vehicle, int tireIndex)
{
    logger->Info("Tire %d of vehicle %d is now flat", tireIndex, vehicle);
}

// Detect control input and handle skid/flat logic
void CheckHandbrakeAndSkid()
{
    int vehicle = GetPlayerVehicle();
    if (vehicle == 0)
        return; // No vehicle, do nothing

    if (IsControlPressed(0, CONTROL_HAND_BRAKE))
    {
        holdCounter += CTimer::m_snTimeInMilliseconds; // accumulate hold time
        if (holdCounter > 2000 && !isSkidding)
        {
            // Start skid
            isSkidding = true;
            skidStartTime = CTimer::m_snTimeInMilliseconds;
            logger->Info("Skid started");
            // Optional: trigger skid animation/effect here
        }
    }
    else
    {
        holdCounter = 0;
        if (isSkidding && !tireFlat)
        {
            // After releasing, convert tire to flat
            int tireIndex = 0; // Choose which tire to flat
            SetTireFlat(vehicle, tireIndex);
            tireFlat = true;
            isSkidding = false;
        }
    }

    // Check skid duration for flat tire
    if (isSkidding && !tireFlat)
    {
        int currentTime = CTimer::m_snTimeInMilliseconds;
        if (currentTime - skidStartTime > skidDuration)
        {
            int tireIndex = 0; // Choose which tire to flat
            SetTireFlat(vehicle, tireIndex);
            tireFlat = true;
            isSkidding = false;
            logger->Info("Tire has gone flat after skid");
        }
    }
}

// Main hook
DECL_HOOKv(CTimer__Update)
{
    CTimer__Update();
    CTimer::m_snTimeInMilliseconds += 5;

    // Custom logic
    CheckHandbrakeAndSkid();

    logger->Info("CTimer__Update: %u", CTimer::m_snTimeInMilliseconds);
    CTimer::m_snTimeInMilliseconds -= 5;
}

ON_MOD_LOAD()
{
    logger->SetTag("SkidMechanic");
    HOOK(CTimer__Update, CTimer::Update);
}
