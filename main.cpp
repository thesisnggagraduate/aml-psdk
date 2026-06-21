#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <aml-psdk/game_sa/base/Timer.h>
#include <aml-psdk/game_sa/controls.h>
#include <aml-psdk/game_sa/player.h>
#include <aml-psdk/game_sa/vehicle.h>

MYMODCFG(net.psdk.mymod.guid, AML PSDK Realistic Overheat, 1.0, Author)

static bool isSkidding = false;
static int skidStartTime = 0;

static bool isOverheated = false;
static int overheatStartTime = 0;

static bool isBlown = false;
static int blowoutTime = 0;

const int skidThreshold = 3000; // 3 seconds
const int overheatThreshold = 180000; // 3 minutes in milliseconds
const int blowoutThreshold = 360000; // 6 minutes in milliseconds (total after start)

// Kumuha ng vehicle ng player
int GetPlayerVehicle()
{
    int ped = PLAYER_PED_ID();
    return GET_VEHICLE_PED_IS_IN(ped, false);
}

// Placeholder function para sa blowout effect
void CreateFireOnTire(int vehicle, int tireIndex)
{
    logger->Info("Fire sa tire %d ng sasakyan %d", tireIndex, vehicle);
}

// Placeholder function para sa blowout ng gulong
void BlowoutTire(int vehicle, int tireIndex)
{
    logger->Info("Gulong %d ng sasakyan %d ay sumabog", tireIndex, vehicle);
    isBlown = true;
    blowoutTime = CTimer::m_snTimeInMilliseconds;
}

// Check control at manage ang skid, overheat, at blowout
void CheckSkidOverheatAndBlowout()
{
    int vehicle = GetPlayerVehicle();
    if (vehicle == 0)
        return; // walang vehicle

    if (IsControlPressed(0, CONTROL_HAND_BRAKE))
    {
        static int holdStartTime = 0;
        if (holdStartTime == 0)
            holdStartTime = CTimer::m_snTimeInMilliseconds;

        int heldTime = CTimer::m_snTimeInMilliseconds - holdStartTime;
        if (heldTime > skidThreshold && !isSkidding)
        {
            // Simula ng skid
            isSkidding = true;
            skidStartTime = CTimer::m_snTimeInMilliseconds;
            isOverheated = false;
            logger->Info("Skid nagsimula");
        }
    }
    else
    {
        // Inalis ang handbrake
        if (isSkidding && !isOverheated)
        {
            int currentTime = CTimer::m_snTimeInMilliseconds;
            int skidDuration = currentTime - skidStartTime;

            // Kapag umabot ng 3 minuto (180,000 ms), magsimula ang overheat
            if (skidDuration > overheatThreshold && !isOverheated)
            {
                isOverheated = true;
                overheatStartTime = CTimer::m_snTimeInMilliseconds;
                logger->Info("Gulong nagsimula umapoy (overheat)");
                // Maaari kang maglagay dito ng visual effect
            }
        }
        // Reset kapag hindi na skid
        holdStartTime = 0;
        isSkidding = false;
    }

    // Kapag overheat na, maghintay ng 6 minuto bago sumabog
    if (isOverheated && !isBlown)
    {
        int currentTime = CTimer::m_snTimeInMilliseconds;
        if (currentTime - overheatStartTime > blowoutThreshold)
        {
            // Sumabog ang gulong
            BlowoutTire(vehicle, 0);
            logger->Info("Gulong sumabog pagkatapos umapoy ng 3 minuto at tumagal pa ng 3 minuto");
        }
    }
}

// Hook function
DECL_HOOKv(CTimer__Update)
{
    CTimer__Update();
    CTimer::m_snTimeInMilliseconds += 5;

    CheckSkidOverheatAndBlowout();

    logger->Info("CTimer__Update: %u", CTimer::m_snTimeInMilliseconds);
    CTimer::m_snTimeInMilliseconds -= 5;
}

ON_MOD_LOAD()
{
    logger->SetTag("RealisticOverheat");
    HOOK(CTimer__Update, CTimer::Update);
}
