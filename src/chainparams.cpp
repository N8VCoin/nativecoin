// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2019 The NativeCoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"
#include "stdio.h"
#include <iostream>
#include "libzerocoin/bignum.h"
#include <assert.h>
#include "script/standard.h"
#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{

    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("0x00000b7892e23de3b819bb38f04cd3ff340e48ad7c379822a4ffa2b6a957eb9c"))
    (500, uint256("0xDF0A736659F6BB7C65422A1559209A2DB4DA065586650DE6251A1741A1BC2757"))
    (1000, uint256("0x8D1287DCEFB1AA71BF6FA973A61F5A537D3BE41B06B4608F39414EE1958D6AAF"))
    (2500, uint256("0x064E52B7419CAAA307FF168B03CA00490B177CD2AB4263F96AA7ED28DE380AF3"))
    (3000, uint256("0x89539BD72E124EE8DB9066B48682D2EBFB1AD3B610BB693E64A8400812AAD9CB"))
    (4500, uint256("0xDE3F9F382AAE8CFE690622E7DF066316A442AE7C6F794565647FF439A1588AA3"))
    (6500, uint256("0xDCDAAA8DB9A24F0F2ED5FF0D112FD145EA3C3FDF6638039112151316E34DEA8D"))
    (8500, uint256("0x748DD3C0FECC470694A6BC06DED92BD7EDBBE945081E2D10ACA5D16DDE9C7C47"))
    (10000, uint256("0x5BA8CC14AA4F92EA1E36B09657F6C7505CA89E43D2BD829AE88A49C370D23393"))
    (13500, uint256("0x4A5DE183340197B6993AD726A2606756AD01683A2A7A0C7488752C65E2F6E5A5"));
  /*(867733, uint256("0x03b26296bf693de5782c76843d2fb649cb66d4b05550c6a79c047ff7e1c3ae15"))
    (879650, uint256("0x227e1d2b738b6cd83c46d1d64617934ec899d77cee34336a56e61b71acd10bb2"))
    (895400, uint256("0x7796a0274a608fac12d400198174e50beda992c1d522e52e5b95b884bc1beac6"))//block that serial# range is enforced
    (895991, uint256("0xd53013ed7ea5c325b9696c95e07667d6858f8ff7ee13fecfa90827bf3c9ae316"))//network split here
    (908000, uint256("0x202708f8c289b676fceb832a079ff6b308a28608339acbf7584de533619d014d"))
    (1142400, uint256("0x98aff9d605bf123247f98b1e3a02567eb5799d208d78ec30fb89737b1c1f79c5"))
    (1679090, uint256("0xf747ce055ba1b12e1f2e842bd480bc647210799359cb2e553ab292065e3419d6")) //!< First block with a "wrapped" serial spend
    (1686229, uint256("0xbb42bf1e886a7c23474634c90893dd3d68a6ccbfea4ac92a98da5cad0c6a6cb7")); //!< Last block in the "wrapped" serial attack range **/
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1558130910,
    4000,

    200};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (0, uint256("0x00000819edb34d126fadb7f6582f64de7466cf35ac5546d3852ad4c6f3ba393d"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1558130910,
    2305594,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x0000080fa3c34854904d593264552fc25ae841844e737ccfb23777e48edfa867"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1558130910,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params(bool useModulusV1) const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);
    static CBigNum bnDecModulus = 0;
    if (!bnDecModulus)
        bnDecModulus.SetDec(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsDec = libzerocoin::ZerocoinParams(bnDecModulus);

    if (useModulusV1)
        return &ZCParamsHex;

    return &ZCParamsDec;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";

        pchMessageStart[0] = 0xc0;
        pchMessageStart[1] = 0x23;
        pchMessageStart[2] = 0x56;
        pchMessageStart[3] = 0xa9;
        vAlertPubKey = ParseHex("049c6f251a8c2cb50002a1d8d54a5959a685a00833e7362b144db4995fc376df0f0e7441674c50c8e16d294e5fb1b4e7790774f491e743de7a07c348ab3bf0b413");
        nDefaultPort = 8848;
        bnProofOfWorkLimit = ~uint256(0) >> 20;
        nSubsidyHalvingInterval = 480000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 8100;
        nRejectBlockOutdatedMajority = 10260;
        nToCheckBlockUpgradeMajority = 10800;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60;
        nTargetSpacing = 1 * 60;
        nMaturity = 30;
        nMasternodeCountDrift = 20;
        nMasternodeCollateralLimit = 25000;
        nMaxMoneyOut = 40000000 * COIN;
        nLastPOWBlock = 200;
        nModifierUpdateBlock = INT_MAX;
        nZerocoinStartHeight = INT_MAX;
        nZerocoinStartTime = INT_MAX;
        nBlockEnforceSerialRange = 100;
        nBlockRecalculateAccumulators = INT_MAX;
        nBlockFirstFraudulent = INT_MAX;
        nBlockLastGoodCheckpoint = INT_MAX;
        nBlockEnforceInvalidUTXO = 902850;
        nBlockZerocoinV2 = INT_MAX;
        nBlockDoubleAccumulated = 1050010;
        nEnforceNewSporkKey = 1425158000;
        nRejectOldSporkKey = 1527811200;

        nFakeSerialBlockheightEnd = 1686229;

        const char* pszTimestamp = "Trump’s Middle East Peace Plan Faces a Crossroads After Coalition Talks in Israel Crumble June 1st 2019";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04e90c6400617178c7d6c7cc4f43ba0be21ed9de494b39401eebc2e79a18a7a55ddd455123aa5d1a5d4e9d5a907e9957ac55d011210e0845a294fa0d7c0a318a8d") << OP_CHECKSIG; 
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1559461295;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 1555191;
        hashGenesisBlock = genesis.GetHash();

        assert(hashGenesisBlock == uint256("0x00000b7892e23de3b819bb38f04cd3ff340e48ad7c379822a4ffa2b6a957eb9c"));
        assert(genesis.hashMerkleRoot == uint256("0xc0ac20b96c09fcfb9b1955cf77d0e00072e46ebca30e25e773c104bd7c2e3dca"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("seed1.n8vcoin.dev", "seed1.n8vcoin.dev"));
        vSeeds.push_back(CDNSSeedData("seed2.n8vcoin.dev", "seed2.n8vcoin.dev"));
        vSeeds.push_back(CDNSSeedData("seed3.n8vcoin.dev", "seed3.n8vcoin.dev"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 53);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 122);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 112);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x17)(0x5D)(0x15)(0xA3).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0xD1)(0xC1)(0x1B).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x01)(0x54).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        nBudgetCycleBlocks = 43200;
        strSporkKey = "04d7e318c46c0252ecde2eef1ab112ccf6e7acf0690da44739d73f147dc5775aaacd568dc4a0fe42ea1d10929a197685cecea2be173f156c109eab9ee21ea33692";
        strObfuscationPoolDummyAddress = "DKv8dUifgBKkWM1nwjad7yNasQ41yA9ntR";
        nStartMasternodePayments = 1403728576;

        zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";
        nMaxZerocoinSpendsPerTransaction = 7;
        nMinZerocoinMintFee = 1 * CENT;
        nMintRequiredConfirmations = 20;
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100;
        nZerocoinHeaderVersion = INT_MAX;
        nZerocoinRequiredStakeDepth = 200;

        nBudget_Fee_Confirmations = 6;
        nProposalEstablishmentTime = 60 * 60 * 24;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};

static CMainParams mainParams;

class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";

        pchMessageStart[0] = 0xa4;
        pchMessageStart[1] = 0xb3;
        pchMessageStart[2] = 0xa6;
        pchMessageStart[3] = 0xd9;
        vAlertPubKey = ParseHex("0455049254ed020015f732bdd6e99cfaaaab7119cbfd6c7c093182926e945733d9b3bb4cf775574c123e005380ed5b6c329f20026c95fdb0b2134614a414cff1fe");
        nDefaultPort = 8884;
        nEnforceBlockUpgradeMajority = 4320;
        nRejectBlockOutdatedMajority = 5472;
        nToCheckBlockUpgradeMajority = 5760;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60;
        nTargetSpacing = 1 * 60; 
        nLastPOWBlock = 120;
        nMaturity = 4;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 51197;
        nMaxMoneyOut = 49000000 * COIN;
        nZerocoinStartHeight = INT_MAX;
        nZerocoinStartTime = INT_MAX;
        nBlockEnforceSerialRange = 1; 
        nBlockRecalculateAccumulators = 9908000; 
        nBlockFirstFraudulent = 9891737; 
        nBlockLastGoodCheckpoint = 9891730; 
        nBlockEnforceInvalidUTXO = 9902850; 
        nInvalidAmountFiltered = 0; 
        nBlockZerocoinV2 = 444020; 
        nEnforceNewSporkKey = 1521604800; 
        nRejectOldSporkKey = 1522454400; 

        nFakeSerialBlockheightEnd = -1;
        nSupplyBeforeFakeSerial = 0;

        genesis.nTime = 1559461244; 
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 44582;


        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000819edb34d126fadb7f6582f64de7466cf35ac5546d3852ad4c6f3ba393d"));
        assert(genesis.hashMerkleRoot == uint256("0xc0ac20b96c09fcfb9b1955cf77d0e00072e46ebca30e25e773c104bd7c2e3dca"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("testnet1.n8vcoin.dev", "testnet1.n8vcoin.dev"));
        vSeeds.push_back(CDNSSeedData("testnet2.n8vcoin.dev", "testnet2.n8vcoin.dev"));
        vSeeds.push_back(CDNSSeedData("testnet3.n8vcoin.dev", "testnet3.n8vcoin.dev"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 35);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 12);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 122);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x05)(0x1D)(0xA5)(0xB3).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x07)(0x2A)(0x3C)(0x2D).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x01)(0x54).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        nBudgetCycleBlocks = 144; 
        strSporkKey = "042068d4cb3ce38c7b7ec189075ad22d9462d2e4837cad43c68fc95a89db002968603605e67c7af656d5e3ea67f7237ba207ffa7c83b1c07e290df49ee21f85834";
        strObfuscationPoolDummyAddress = "xp87cG8UEQgzs1Bk67Yk884C7pnQfAeo7q";
        nStartMasternodePayments = 1520837558; 
        nBudget_Fee_Confirmations = 3; 


        nProposalEstablishmentTime = 60 * 5;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xa2;
        pchMessageStart[1] = 0x13;
        pchMessageStart[2] = 0x36;
        pchMessageStart[3] = 0xA9;
        nDefaultPort = 8888;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 24 * 60 * 60; 
        nTargetSpacing = 1 * 60;        
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        nLastPOWBlock = 250;
        nMaturity = 100;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 0; 
        nMaxMoneyOut = 44000000 * COIN;
        nZerocoinStartHeight = INT_MAX;
        nBlockZerocoinV2 = 300;
        nZerocoinStartTime = INT_MAX;
        nBlockEnforceSerialRange = 1; 
        nBlockRecalculateAccumulators = 999999999; 
        nBlockFirstFraudulent = 999999999; 
        nBlockLastGoodCheckpoint = 999999999; 


        nFakeSerialBlockheightEnd = -1;

        genesis.nTime = 1559461175; 
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 1883825;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x0000080fa3c34854904d593264552fc25ae841844e737ccfb23777e48edfa867"));
        assert(genesis.hashMerkleRoot == uint256("0xc0ac20b96c09fcfb9b1955cf77d0e00072e46ebca30e25e773c104bd7c2e3dca"));

        vFixedSeeds.clear();
        vSeeds.clear();

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;


class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 51478;
        vFixedSeeds.clear(); 
        vSeeds.clear();      

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
    
        return data;
    }

    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
