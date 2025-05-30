#include <assert.h>
#include "behavior_table.h"
#include "pc/lua/smlua_hooks.h"

struct BehaviorTableEntry {
    enum BehaviorId id;
    const BehaviorScript* script;
    const char* name;
};

#define BHV_ENTRY(BHV) [id_##BHV] = { .id = id_##BHV, .script = BHV, .name = #BHV }

const struct BehaviorTableEntry gBehaviorTable[id_bhv_max_count] = {
    BHV_ENTRY(bhv1Up),
    BHV_ENTRY(bhv1upJumpOnApproach),
    BHV_ENTRY(bhv1upRunningAway),
    BHV_ENTRY(bhv1upSliding),
    BHV_ENTRY(bhv1upWalking),
    BHV_ENTRY(bhvActivatedBackAndForthPlatform),
    BHV_ENTRY(bhvActSelector),
    BHV_ENTRY(bhvActSelectorStarType),
    BHV_ENTRY(bhvAirborneDeathWarp),
    BHV_ENTRY(bhvAirborneStarCollectWarp),
    BHV_ENTRY(bhvAirborneWarp),
    BHV_ENTRY(bhvAlphaBooKey),
    BHV_ENTRY(bhvAmbientSounds),
    BHV_ENTRY(bhvAnimatedTexture),
    BHV_ENTRY(bhvAnimatesOnFloorSwitchPress),
    BHV_ENTRY(bhvAnotherElavator),
    BHV_ENTRY(bhvAnotherTiltingPlatform),
    BHV_ENTRY(bhvArrowLift),
    BHV_ENTRY(bhvBalconyBigBoo),
    BHV_ENTRY(bhvBbhTiltingTrapPlatform),
    BHV_ENTRY(bhvBbhTumblingBridge),
    BHV_ENTRY(bhvBeginningLakitu),
    BHV_ENTRY(bhvBeginningPeach),
    BHV_ENTRY(bhvBetaBooKey),
    BHV_ENTRY(bhvBetaBowserAnchor),
    BHV_ENTRY(bhvBetaChestBottom),
    BHV_ENTRY(bhvBetaChestLid),
    BHV_ENTRY(bhvBetaFishSplashSpawner),
    BHV_ENTRY(bhvBetaHoldableObject),
    BHV_ENTRY(bhvBetaMovingFlames),
    BHV_ENTRY(bhvBetaMovingFlamesSpawn),
    BHV_ENTRY(bhvBetaTrampolineSpring),
    BHV_ENTRY(bhvBetaTrampolineTop),
    BHV_ENTRY(bhvBigBoulder),
    BHV_ENTRY(bhvBigBoulderGenerator),
    BHV_ENTRY(bhvBigBully),
    BHV_ENTRY(bhvBigBullyWithMinions),
    BHV_ENTRY(bhvBigChillBully),
    BHV_ENTRY(bhvBigSnowmanWhole),
    BHV_ENTRY(bhvBird),
    BHV_ENTRY(bhvBirdsSoundLoop),
    BHV_ENTRY(bhvBitfsSinkingCagePlatform),
    BHV_ENTRY(bhvBitfsSinkingPlatforms),
    BHV_ENTRY(bhvBitfsTiltingInvertedPyramid),
    BHV_ENTRY(bhvBlackSmokeBowser),
    BHV_ENTRY(bhvBlackSmokeMario),
    BHV_ENTRY(bhvBlackSmokeUpward),
    BHV_ENTRY(bhvBlueBowserFlame),
    BHV_ENTRY(bhvBlueCoinJumping),
    BHV_ENTRY(bhvBlueCoinSliding),
    BHV_ENTRY(bhvBlueCoinNumber),
    BHV_ENTRY(bhvBlueCoinSwitch),
    BHV_ENTRY(bhvBlueFish),
    BHV_ENTRY(bhvBlueFlamesGroup),
    BHV_ENTRY(bhvBobBowlingBallSpawner),
    BHV_ENTRY(bhvBobomb),
    BHV_ENTRY(bhvBobombAnchorMario),
    BHV_ENTRY(bhvBobombBuddy),
    BHV_ENTRY(bhvBobombBuddyOpensCannon),
    BHV_ENTRY(bhvBobombBullyDeathSmoke),
    BHV_ENTRY(bhvBobombExplosionBubble),
    BHV_ENTRY(bhvBobombExplosionBubble3600),
    BHV_ENTRY(bhvBobombFuseSmoke),
    BHV_ENTRY(bhvBoo),
    BHV_ENTRY(bhvBooBossSpawnedBridge),
    BHV_ENTRY(bhvBooCage),
    BHV_ENTRY(bhvBooInCastle),
    BHV_ENTRY(bhvBookendSpawn),
    BHV_ENTRY(bhvBookSwitch),
    BHV_ENTRY(bhvBooWithCage),
    BHV_ENTRY(bhvBouncingFireball),
    BHV_ENTRY(bhvBouncingFireballFlame),
    BHV_ENTRY(bhvBowlingBall),
    BHV_ENTRY(bhvBowser),
    BHV_ENTRY(bhvBowserBodyAnchor),
    BHV_ENTRY(bhvBowserBomb),
    BHV_ENTRY(bhvBowserBombExplosion),
    BHV_ENTRY(bhvBowserBombSmoke),
    BHV_ENTRY(bhvBowserCourseRedCoinStar),
    BHV_ENTRY(bhvBowserFlameSpawn),
    BHV_ENTRY(bhvBowserKey),
    BHV_ENTRY(bhvBowserKeyCourseExit),
    BHV_ENTRY(bhvBowserKeyUnlockDoor),
    BHV_ENTRY(bhvBowserShockWave),
    BHV_ENTRY(bhvBowsersSub),
    BHV_ENTRY(bhvBowserSubDoor),
    BHV_ENTRY(bhvBowserTailAnchor),
    BHV_ENTRY(bhvBreakableBox),
    BHV_ENTRY(bhvBreakableBoxSmall),
    BHV_ENTRY(bhvBreakBoxTriangle),
    BHV_ENTRY(bhvBreathParticleSpawner),
    BHV_ENTRY(bhvBub),
    BHV_ENTRY(bhvBubba),
    BHV_ENTRY(bhvBubbleMaybe),
    BHV_ENTRY(bhvBubbleParticleSpawner),
    BHV_ENTRY(bhvBubblePlayer),
    BHV_ENTRY(bhvBubbleSplash),
    BHV_ENTRY(bhvBulletBill),
    BHV_ENTRY(bhvBulletBillCannon),
    BHV_ENTRY(bhvButterfly),
    BHV_ENTRY(bhvCameraLakitu),
    BHV_ENTRY(bhvCannon),
    BHV_ENTRY(bhvCannonBarrel),
    BHV_ENTRY(bhvCannonBarrelBubbles),
    BHV_ENTRY(bhvCannonBaseUnused),
    BHV_ENTRY(bhvCannonClosed),
    BHV_ENTRY(bhvCapSwitch),
    BHV_ENTRY(bhvCapSwitchBase),
    BHV_ENTRY(bhvCarrySomething1),
    BHV_ENTRY(bhvCarrySomething2),
    BHV_ENTRY(bhvCarrySomething3),
    BHV_ENTRY(bhvCarrySomething4),
    BHV_ENTRY(bhvCarrySomething5),
    BHV_ENTRY(bhvCarrySomething6),
    BHV_ENTRY(bhvCastleFlagWaving),
    BHV_ENTRY(bhvCastleFloorTrap),
    BHV_ENTRY(bhvCcmTouchedStarSpawn),
    BHV_ENTRY(bhvCelebrationStar),
    BHV_ENTRY(bhvCelebrationStarSparkle),
    BHV_ENTRY(bhvChainChomp),
    BHV_ENTRY(bhvChainChompChainPart),
    BHV_ENTRY(bhvChainChompGate),
    BHV_ENTRY(bhvCheckerboardElevatorGroup),
    BHV_ENTRY(bhvCheckerboardPlatformSub),
    BHV_ENTRY(bhvChirpChirp),
    BHV_ENTRY(bhvChirpChirpUnused),
    BHV_ENTRY(bhvChuckya),
    BHV_ENTRY(bhvChuckyaAnchorMario),
    BHV_ENTRY(bhvCirclingAmp),
    BHV_ENTRY(bhvClamShell),
    BHV_ENTRY(bhvClockHourHand),
    BHV_ENTRY(bhvClockMinuteHand),
    BHV_ENTRY(bhvCloud),
    BHV_ENTRY(bhvCloudPart),
    BHV_ENTRY(bhvCoffin),
    BHV_ENTRY(bhvCoffinSpawner),
    BHV_ENTRY(bhvCoinFormation),
    BHV_ENTRY(bhvCoinFormationSpawn),
    BHV_ENTRY(bhvCoinInsideBoo),
    BHV_ENTRY(bhvCoinSparkles),
    BHV_ENTRY(bhvControllablePlatform),
    BHV_ENTRY(bhvControllablePlatformSub),
    BHV_ENTRY(bhvCourtyardBooTriplet),
    BHV_ENTRY(bhvCutOutObject),
    BHV_ENTRY(bhvDddMovingPole),
    BHV_ENTRY(bhvDDDPole),
    BHV_ENTRY(bhvDddWarp),
    BHV_ENTRY(bhvDeathWarp),
    BHV_ENTRY(bhvDecorativePendulum),
    BHV_ENTRY(bhvDirtParticleSpawner),
    BHV_ENTRY(bhvDonutPlatform),
    BHV_ENTRY(bhvDonutPlatformSpawner),
    BHV_ENTRY(bhvDoor),
    BHV_ENTRY(bhvDoorWarp),
    BHV_ENTRY(bhvDorrie),
    BHV_ENTRY(bhvEndBirds1),
    BHV_ENTRY(bhvEndBirds2),
    BHV_ENTRY(bhvEndPeach),
    BHV_ENTRY(bhvEndToad),
    BHV_ENTRY(bhvEnemyLakitu),
    BHV_ENTRY(bhvExclamationBox),
    BHV_ENTRY(bhvExitPodiumWarp),
    BHV_ENTRY(bhvExplosion),
    BHV_ENTRY(bhvEyerokBoss),
    BHV_ENTRY(bhvEyerokHand),
    BHV_ENTRY(bhvFadingWarp),
    BHV_ENTRY(bhvFallingBowserPlatform),
    BHV_ENTRY(bhvFallingPillar),
    BHV_ENTRY(bhvFallingPillarHitbox),
    BHV_ENTRY(bhvFerrisWheelAxle),
    BHV_ENTRY(bhvFerrisWheelPlatform),
    BHV_ENTRY(bhvFewBlueFishSpawner),
    BHV_ENTRY(bhvFireParticleSpawner),
    BHV_ENTRY(bhvFirePiranhaPlant),
    BHV_ENTRY(bhvFireSpitter),
    BHV_ENTRY(bhvFish),
    BHV_ENTRY(bhvFishGroup),
    BHV_ENTRY(bhvFishSpawner),
    BHV_ENTRY(bhvFlame),
    BHV_ENTRY(bhvFlameBouncing),
    BHV_ENTRY(bhvFlameBowser),
    BHV_ENTRY(bhvFlameFloatingLanding),
    BHV_ENTRY(bhvFlameLargeBurningOut),
    BHV_ENTRY(bhvFlameMovingForwardGrowing),
    BHV_ENTRY(bhvFlamethrower),
    BHV_ENTRY(bhvFlamethrowerFlame),
    BHV_ENTRY(bhvFloorSwitchAnimatesObject),
    BHV_ENTRY(bhvFloorSwitchGrills),
    BHV_ENTRY(bhvFloorSwitchHardcodedModel),
    BHV_ENTRY(bhvFloorSwitchHiddenObjects),
    BHV_ENTRY(bhvFloorTrapInCastle),
    BHV_ENTRY(bhvFlyGuy),
    BHV_ENTRY(bhvFlyguyFlame),
    BHV_ENTRY(bhvFlyingBookend),
    BHV_ENTRY(bhvFlyingWarp),
    BHV_ENTRY(bhvFreeBowlingBall),
    BHV_ENTRY(bhvGhostHuntBigBoo),
    BHV_ENTRY(bhvGhostHuntBoo),
    BHV_ENTRY(bhvGiantPole),
    BHV_ENTRY(bhvGoldenCoinSparkles),
    BHV_ENTRY(bhvGoomba),
    BHV_ENTRY(bhvGoombaTripletSpawner),
    BHV_ENTRY(bhvGrandStar),
    BHV_ENTRY(bhvGrindel),
    BHV_ENTRY(bhvHardAirKnockBackWarp),
    BHV_ENTRY(bhvHauntedBookshelf),
    BHV_ENTRY(bhvHauntedBookshelfManager),
    BHV_ENTRY(bhvHauntedChair),
    BHV_ENTRY(bhvHeaveHo),
    BHV_ENTRY(bhvHeaveHoThrowMario),
    BHV_ENTRY(bhvHidden1up),
    BHV_ENTRY(bhvHidden1upInPole),
    BHV_ENTRY(bhvHidden1upInPoleSpawner),
    BHV_ENTRY(bhvHidden1upInPoleTrigger),
    BHV_ENTRY(bhvHidden1upTrigger),
    BHV_ENTRY(bhvHiddenAt120Stars),
    BHV_ENTRY(bhvHiddenBlueCoin),
    BHV_ENTRY(bhvHiddenObject),
    BHV_ENTRY(bhvHiddenRedCoinStar),
    BHV_ENTRY(bhvHiddenStaircaseStep),
    BHV_ENTRY(bhvHiddenStar),
    BHV_ENTRY(bhvHiddenStarTrigger),
    BHV_ENTRY(bhvHmcElevatorPlatform),
    BHV_ENTRY(bhvHomingAmp),
    BHV_ENTRY(bhvHoot),
    BHV_ENTRY(bhvHorizontalGrindel),
    BHV_ENTRY(bhvHorStarParticleSpawner),
    BHV_ENTRY(bhvIdleWaterWave),
    BHV_ENTRY(bhvIgloo),
    BHV_ENTRY(bhvInitializeChangingWaterLevel),
    BHV_ENTRY(bhvInsideCannon),
    BHV_ENTRY(bhvInstantActiveWarp),
    BHV_ENTRY(bhvInSunkenShip),
    BHV_ENTRY(bhvInSunkenShip2),
    BHV_ENTRY(bhvInSunkenShip3),
    BHV_ENTRY(bhvIntroScene),
    BHV_ENTRY(bhvInvisibleObjectsUnderBridge),
    BHV_ENTRY(bhvJetStream),
    BHV_ENTRY(bhvJetStreamRingSpawner),
    BHV_ENTRY(bhvJetStreamWaterRing),
    BHV_ENTRY(bhvJrbFloatingBox),
    BHV_ENTRY(bhvJrbFloatingPlatform),
    BHV_ENTRY(bhvJrbSlidingBox),
    BHV_ENTRY(bhvJumpingBox),
    BHV_ENTRY(bhvKickableBoard),
    BHV_ENTRY(bhvKingBobomb),
    BHV_ENTRY(bhvKlepto),
    BHV_ENTRY(bhvKoopa),
    BHV_ENTRY(bhvKoopaFlag),
    BHV_ENTRY(bhvKoopaRaceEndpoint),
    BHV_ENTRY(bhvKoopaShell),
    BHV_ENTRY(bhvKoopaShellFlame),
    BHV_ENTRY(bhvKoopaShellUnderwater),
    BHV_ENTRY(bhvLargeBomp),
    BHV_ENTRY(bhvLaunchDeathWarp),
    BHV_ENTRY(bhvLaunchStarCollectWarp),
    BHV_ENTRY(bhvLeafParticleSpawner),
    BHV_ENTRY(bhvLllBowserPuzzle),
    BHV_ENTRY(bhvLllBowserPuzzlePiece),
    BHV_ENTRY(bhvLllDrawbridge),
    BHV_ENTRY(bhvLllDrawbridgeSpawner),
    BHV_ENTRY(bhvLllFloatingWoodBridge),
    BHV_ENTRY(bhvLllHexagonalMesh),
    BHV_ENTRY(bhvLllMovingOctagonalMeshPlatform),
    BHV_ENTRY(bhvLllRollingLog),
    BHV_ENTRY(bhvLllRotatingBlockWithFireBars),
    BHV_ENTRY(bhvLllRotatingHexagonalPlatform),
    BHV_ENTRY(bhvLllRotatingHexagonalRing),
    BHV_ENTRY(bhvLllRotatingHexFlame),
    BHV_ENTRY(bhvLllSinkingRectangularPlatform),
    BHV_ENTRY(bhvLllSinkingRockBlock),
    BHV_ENTRY(bhvLllSinkingSquarePlatforms),
    BHV_ENTRY(bhvLllTiltingInvertedPyramid),
    BHV_ENTRY(bhvLllTumblingBridge),
    BHV_ENTRY(bhvLllVolcanoFallingTrap),
    BHV_ENTRY(bhvLllWoodPiece),
    BHV_ENTRY(bhvMacroUkiki),
    BHV_ENTRY(bhvMadPiano),
    BHV_ENTRY(bhvMantaRay),
    BHV_ENTRY(bhvMantaRayRingManager),
    BHV_ENTRY(bhvMantaRayWaterRing),
    BHV_ENTRY(bhvManyBlueFishSpawner),
    BHV_ENTRY(bhvMario),
    BHV_ENTRY(bhvMenuButton),
    BHV_ENTRY(bhvMenuButtonManager),
    BHV_ENTRY(bhvMerryGoRound),
    BHV_ENTRY(bhvMerryGoRoundBigBoo),
    BHV_ENTRY(bhvMerryGoRoundBoo),
    BHV_ENTRY(bhvMerryGoRoundBooManager),
    BHV_ENTRY(bhvMeshElevator),
    BHV_ENTRY(bhvMessagePanel),
    BHV_ENTRY(bhvMetalCap),
    BHV_ENTRY(bhvMips),
    BHV_ENTRY(bhvMistCircParticleSpawner),
    BHV_ENTRY(bhvMistParticleSpawner),
    BHV_ENTRY(bhvMoatGrills),
    BHV_ENTRY(bhvMoneybag),
    BHV_ENTRY(bhvMoneybagHidden),
    BHV_ENTRY(bhvMontyMole),
    BHV_ENTRY(bhvMontyMoleHole),
    BHV_ENTRY(bhvMontyMoleRock),
    BHV_ENTRY(bhvMovingBlueCoin),
    BHV_ENTRY(bhvMovingYellowCoin),
    BHV_ENTRY(bhvMrBlizzard),
    BHV_ENTRY(bhvMrBlizzardSnowball),
    BHV_ENTRY(bhvMrI),
    BHV_ENTRY(bhvMrIBlueCoin),
    BHV_ENTRY(bhvMrIBody),
    BHV_ENTRY(bhvMrIParticle),
    BHV_ENTRY(bhvNormalCap),
    BHV_ENTRY(bhvObjectBubble),
    BHV_ENTRY(bhvObjectWaterSplash),
    BHV_ENTRY(bhvObjectWaterWave),
    BHV_ENTRY(bhvObjectWaveTrail),
    BHV_ENTRY(bhvOctagonalPlatformRotating),
    BHV_ENTRY(bhvOneCoin),
    BHV_ENTRY(bhvOpenableCageDoor),
    BHV_ENTRY(bhvOpenableGrill),
    BHV_ENTRY(bhvOrangeNumber),
    BHV_ENTRY(bhvPaintingDeathWarp),
    BHV_ENTRY(bhvPaintingStarCollectWarp),
    BHV_ENTRY(bhvPenguinBaby),
    BHV_ENTRY(bhvPenguinRaceFinishLine),
    BHV_ENTRY(bhvPenguinRaceShortcutCheck),
    BHV_ENTRY(bhvPillarBase),
    BHV_ENTRY(bhvPiranhaPlant),
    BHV_ENTRY(bhvPiranhaPlantBubble),
    BHV_ENTRY(bhvPiranhaPlantWakingBubbles),
    BHV_ENTRY(bhvPitBowlingBall),
    BHV_ENTRY(bhvPlatformOnTrack),
#ifndef VERSION_JP
    BHV_ENTRY(bhvPlaysMusicTrackWhenTouched),
#endif
    BHV_ENTRY(bhvPlungeBubble),
    BHV_ENTRY(bhvPokey),
    BHV_ENTRY(bhvPokeyBodyPart),
    BHV_ENTRY(bhvPoleGrabbing),
    BHV_ENTRY(bhvPoundTinyStarParticle),
    BHV_ENTRY(bhvPunchTinyTriangle),
    BHV_ENTRY(bhvPurpleParticle),
    BHV_ENTRY(bhvPurpleSwitchHiddenBoxes),
    BHV_ENTRY(bhvPushableMetalBox),
    BHV_ENTRY(bhvPyramidElevator),
    BHV_ENTRY(bhvPyramidElevatorTrajectoryMarkerBall),
    BHV_ENTRY(bhvPyramidPillarTouchDetector),
    BHV_ENTRY(bhvPyramidTop),
    BHV_ENTRY(bhvPyramidTopFragment),
    BHV_ENTRY(bhvRacingPenguin),
    BHV_ENTRY(bhvRandomAnimatedTexture),
    BHV_ENTRY(bhvRecoveryHeart),
    BHV_ENTRY(bhvRedCoin),
    BHV_ENTRY(bhvRedCoinStarMarker),
    BHV_ENTRY(bhvRespawner),
    BHV_ENTRY(bhvRockSolid),
    BHV_ENTRY(bhvRotatingCounterClockwise),
    BHV_ENTRY(bhvRotatingExclamationMark),
    BHV_ENTRY(bhvRotatingPlatform),
    BHV_ENTRY(bhvRrCruiserWing),
    BHV_ENTRY(bhvRrElevatorPlatform),
    BHV_ENTRY(bhvRrRotatingBridgePlatform),
    BHV_ENTRY(bhvSandSoundLoop),
    BHV_ENTRY(bhvScuttlebug),
    BHV_ENTRY(bhvScuttlebugSpawn),
    BHV_ENTRY(bhvSeaweed),
    BHV_ENTRY(bhvSeaweedBundle),
    BHV_ENTRY(bhvSeesawPlatform),
    BHV_ENTRY(bhvShallowWaterSplash),
    BHV_ENTRY(bhvShallowWaterWave),
    BHV_ENTRY(bhvShipPart3),
    BHV_ENTRY(bhvSignOnWall),
    BHV_ENTRY(bhvSingleCoinGetsSpawned),
    BHV_ENTRY(bhvSkeeter),
    BHV_ENTRY(bhvSkeeterWave),
    BHV_ENTRY(bhvSlidingPlatform2),
    BHV_ENTRY(bhvSlidingSnowMound),
    BHV_ENTRY(bhvSLSnowmanWind),
    BHV_ENTRY(bhvSLWalkingPenguin),
    BHV_ENTRY(bhvSmallBomp),
    BHV_ENTRY(bhvSmallBully),
    BHV_ENTRY(bhvSmallChillBully),
    BHV_ENTRY(bhvSmallParticle),
    BHV_ENTRY(bhvSmallParticleBubbles),
    BHV_ENTRY(bhvSmallParticleSnow),
    BHV_ENTRY(bhvSmallPenguin),
    BHV_ENTRY(bhvSmallPiranhaFlame),
    BHV_ENTRY(bhvSmallWaterWave),
    BHV_ENTRY(bhvSmallWaterWave398),
    BHV_ENTRY(bhvSmallWhomp),
    BHV_ENTRY(bhvSmoke),
    BHV_ENTRY(bhvSnowBall),
    BHV_ENTRY(bhvSnowmansBodyCheckpoint),
    BHV_ENTRY(bhvSnowmansBottom),
    BHV_ENTRY(bhvSnowmansHead),
    BHV_ENTRY(bhvSnowMoundSpawn),
    BHV_ENTRY(bhvSnowParticleSpawner),
    BHV_ENTRY(bhvSnufit),
    BHV_ENTRY(bhvSnufitBalls),
    BHV_ENTRY(bhvSoundSpawner),
    BHV_ENTRY(bhvSparkle),
    BHV_ENTRY(bhvSparkleParticleSpawner),
    BHV_ENTRY(bhvSparkleSpawn),
    BHV_ENTRY(bhvSpawnedStar),
    BHV_ENTRY(bhvSpawnedStarNoLevelExit),
    BHV_ENTRY(bhvSpinAirborneCircleWarp),
    BHV_ENTRY(bhvSpinAirborneWarp),
    BHV_ENTRY(bhvSpindel),
    BHV_ENTRY(bhvSpindrift),
    BHV_ENTRY(bhvSpiny),
    BHV_ENTRY(bhvSquarishPathMoving),
    BHV_ENTRY(bhvSquarishPathParent),
    BHV_ENTRY(bhvSquishablePlatform),
    BHV_ENTRY(bhvSslMovingPyramidWall),
    BHV_ENTRY(bhvStar),
    BHV_ENTRY(bhvStarDoor),
    BHV_ENTRY(bhvStarKeyCollectionPuffSpawner),
    BHV_ENTRY(bhvStarNumber),
    BHV_ENTRY(bhvStarSpawnCoordinates),
    BHV_ENTRY(bhvStaticCheckeredPlatform),
    BHV_ENTRY(bhvStaticObject),
    BHV_ENTRY(bhvStrongWindParticle),
    BHV_ENTRY(bhvStub),
    BHV_ENTRY(bhvStub1D0C),
    BHV_ENTRY(bhvStub1D70),
    BHV_ENTRY(bhvSunkenShipPart),
    BHV_ENTRY(bhvSunkenShipPart2),
    BHV_ENTRY(bhvSunkenShipSetRotation),
    BHV_ENTRY(bhvSushiShark),
    BHV_ENTRY(bhvSushiSharkCollisionChild),
    BHV_ENTRY(bhvSwimmingWarp),
    BHV_ENTRY(bhvSwingPlatform),
    BHV_ENTRY(bhvSwoop),
    BHV_ENTRY(bhvTankFishGroup),
    BHV_ENTRY(bhvTemporaryYellowCoin),
    BHV_ENTRY(bhvTenCoinsSpawn),
    BHV_ENTRY(bhvThiBowlingBallSpawner),
    BHV_ENTRY(bhvThiHugeIslandTop),
    BHV_ENTRY(bhvThiTinyIslandTop),
    BHV_ENTRY(bhvThreeCoinsSpawn),
    BHV_ENTRY(bhvThwomp),
    BHV_ENTRY(bhvThwomp2),
    BHV_ENTRY(bhvTiltingBowserLavaPlatform),
    BHV_ENTRY(bhvTinyStrongWindParticle),
    BHV_ENTRY(bhvToadMessage),
    BHV_ENTRY(bhvTower),
    BHV_ENTRY(bhvTowerDoor),
    BHV_ENTRY(bhvTowerPlatformGroup),
    BHV_ENTRY(bhvToxBox),
    BHV_ENTRY(bhvTrackBall),
    BHV_ENTRY(bhvTreasureChestBottom),
    BHV_ENTRY(bhvTreasureChests),
    BHV_ENTRY(bhvTreasureChestsJrb),
    BHV_ENTRY(bhvTreasureChestsShip),
    BHV_ENTRY(bhvTreasureChestTop),
    BHV_ENTRY(bhvTree),
    BHV_ENTRY(bhvTreeLeaf),
    BHV_ENTRY(bhvTreeSnow),
    BHV_ENTRY(bhvTriangleParticleSpawner),
    BHV_ENTRY(bhvTripletButterfly),
    BHV_ENTRY(bhvTTC2DRotator),
    BHV_ENTRY(bhvTTCCog),
    BHV_ENTRY(bhvTTCElevator),
    BHV_ENTRY(bhvTTCMovingBar),
    BHV_ENTRY(bhvTTCPendulum),
    BHV_ENTRY(bhvTTCPitBlock),
    BHV_ENTRY(bhvTTCRotatingSolid),
    BHV_ENTRY(bhvTTCSpinner),
    BHV_ENTRY(bhvTTCTreadmill),
    BHV_ENTRY(bhvTtmBowlingBallSpawner),
    BHV_ENTRY(bhvTtmRollingLog),
    BHV_ENTRY(bhvTumblingBridgePlatform),
    BHV_ENTRY(bhvTuxiesMother),
    BHV_ENTRY(bhvTweester),
    BHV_ENTRY(bhvTweesterSandParticle),
    BHV_ENTRY(bhvUkiki),
    BHV_ENTRY(bhvUkikiCage),
    BHV_ENTRY(bhvUkikiCageChild),
    BHV_ENTRY(bhvUkikiCageStar),
    BHV_ENTRY(bhvUnagi),
    BHV_ENTRY(bhvUnagiSubobject),
    BHV_ENTRY(bhvUnlockDoorStar),
    BHV_ENTRY(bhvUnused05A8),
    BHV_ENTRY(bhvUnused0DFC),
    BHV_ENTRY(bhvUnused1820),
    BHV_ENTRY(bhvUnused1F30),
    BHV_ENTRY(bhvUnused20E0),
    BHV_ENTRY(bhvUnused2A10),
    BHV_ENTRY(bhvUnused2A54),
    BHV_ENTRY(bhvUnusedFakeStar),
    BHV_ENTRY(bhvUnusedParticleSpawn),
    BHV_ENTRY(bhvUnusedPoundablePlatform),
    BHV_ENTRY(bhvVanishCap),
    BHV_ENTRY(bhvVertStarParticleSpawner),
    BHV_ENTRY(bhvVolcanoFlames),
    BHV_ENTRY(bhvVolcanoSoundLoop),
    BHV_ENTRY(bhvWallTinyStarParticle),
    BHV_ENTRY(bhvWarp),
    BHV_ENTRY(bhvWarpPipe),
    BHV_ENTRY(bhvWaterAirBubble),
    BHV_ENTRY(bhvWaterBomb),
    BHV_ENTRY(bhvWaterBombCannon),
    BHV_ENTRY(bhvWaterBombShadow),
    BHV_ENTRY(bhvWaterBombSpawner),
    BHV_ENTRY(bhvWaterDroplet),
    BHV_ENTRY(bhvWaterDropletSplash),
    BHV_ENTRY(bhvWaterfallSoundLoop),
    BHV_ENTRY(bhvWaterLevelDiamond),
    BHV_ENTRY(bhvWaterLevelPillar),
    BHV_ENTRY(bhvWaterMist),
    BHV_ENTRY(bhvWaterMist2),
    BHV_ENTRY(bhvWaterSplash),
    BHV_ENTRY(bhvWaveTrail),
    BHV_ENTRY(bhvWdwExpressElevator),
    BHV_ENTRY(bhvWdwExpressElevatorPlatform),
    BHV_ENTRY(bhvWdwRectangularFloatingPlatform),
    BHV_ENTRY(bhvWdwSquareFloatingPlatform),
    BHV_ENTRY(bhvWfBreakableWallLeft),
    BHV_ENTRY(bhvWfBreakableWallRight),
    BHV_ENTRY(bhvWfElevatorTowerPlatform),
    BHV_ENTRY(bhvWfRotatingWoodenPlatform),
    BHV_ENTRY(bhvWfSlidingPlatform),
    BHV_ENTRY(bhvWfSlidingTowerPlatform),
    BHV_ENTRY(bhvWfSolidTowerPlatform),
    BHV_ENTRY(bhvWfTumblingBridge),
    BHV_ENTRY(bhvWhirlpool),
    BHV_ENTRY(bhvWhitePuff1),
    BHV_ENTRY(bhvWhitePuff2),
    BHV_ENTRY(bhvWhitePuffExplosion),
    BHV_ENTRY(bhvWhitePuffSmoke),
    BHV_ENTRY(bhvWhitePuffSmoke2),
    BHV_ENTRY(bhvWhompKingBoss),
    BHV_ENTRY(bhvWigglerBody),
    BHV_ENTRY(bhvWigglerHead),
    BHV_ENTRY(bhvWind),
    BHV_ENTRY(bhvWingCap),
    BHV_ENTRY(bhvWoodenPost),
    BHV_ENTRY(bhvYellowBackgroundInMenu),
    BHV_ENTRY(bhvYellowBall),
    BHV_ENTRY(bhvYellowCoin),
    BHV_ENTRY(bhvYoshi),
    BHV_ENTRY(RM_Scroll_Texture),
    BHV_ENTRY(editor_Scroll_Texture),
    BHV_ENTRY(bhvAmbientLight),
    BHV_ENTRY(bhvPointLight)
};

enum BehaviorId get_id_from_behavior(const BehaviorScript* behavior) {
    if (behavior == NULL) { return id_bhv_max_count; }
    return (enum BehaviorId)(behavior[1] & 0xFFFF);
}

enum BehaviorId get_id_from_vanilla_behavior(const BehaviorScript* behavior) {
    for (enum BehaviorId i = 0; i < id_bhv_max_count; i++) {
        if (gBehaviorTable[i].script == behavior) {
            return i;
        }
    }
    return id_bhv_max_count;
}

const BehaviorScript* get_behavior_from_id(enum BehaviorId id) {
    const BehaviorScript* behavior = smlua_get_hooked_behavior_from_id(id, true);
    if (behavior != NULL) { return behavior; }

    if (id < 0 || id >= id_bhv_max_count) {
        return NULL;
    }

    return gBehaviorTable[id].script;
}

const char* get_behavior_name_from_id(enum BehaviorId id) {
    if (id < 0 || id >= id_bhv_max_count) {
        return smlua_get_name_from_hooked_behavior_id(id);
    }

    return gBehaviorTable[id].name;
}

enum BehaviorId get_id_from_behavior_name(const char* name) {
    for (enum BehaviorId i = 0; i < id_bhv_max_count; i++) {
        if (gBehaviorTable[i].name && !strcmp(name, gBehaviorTable[i].name)) {
            return i;
        }
    }
    return id_bhv_max_count;
}
