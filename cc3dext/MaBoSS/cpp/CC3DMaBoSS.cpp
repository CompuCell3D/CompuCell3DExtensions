#include <chrono>
#include <cstdio>
#include <stdlib.h>

#include "CC3DMaBoSS.h"

extern FILE* CTBNDLin;

using namespace MaBoSSCC3D;

// CC3DRandomGenerator

CC3DRandomGenerator::CC3DRandomGenerator()
{
    seed = 0;
    // Currently MERSENNE_TWISTER seeding does not work
    randGen = RandomGeneratorFactory(RandomGeneratorFactory::DEFAULT).generateRandomGenerator(seed);
}

CC3DRandomGenerator::CC3DRandomGenerator(const int& _seed) 
{
    seed = _seed;
    randGen = RandomGeneratorFactory(RandomGeneratorFactory::DEFAULT).generateRandomGenerator(seed);
}

CC3DRandomGenerator::CC3DRandomGenerator(const CC3DRandomGenerator& other)
{
    seed = other.getSeed();
    randGen = RandomGeneratorFactory(RandomGeneratorFactory::DEFAULT).generateRandomGenerator(seed);
}

CC3DRandomGenerator::~CC3DRandomGenerator()
{
    delete randGen;
    randGen = 0;
}

std::string CC3DRandomGenerator::getName() const {
    return "CC3D";
}

bool CC3DRandomGenerator::isPseudoRandom() const {
    return true;
}

unsigned int CC3DRandomGenerator::generateUInt32() {
    return randGen->generateUInt32();
}

double CC3DRandomGenerator::generate() {
    return randGen->generate();
}

void CC3DRandomGenerator::setSeed(const int &_seed)
{
    seed = _seed;
    randGen->setSeed(_seed);
}

int CC3DRandomGenerator::getSeed() const { return seed; }

RandomGenerator* CC3DRandomGenerator::getRandomGenerator() {
    return randGen;
}

// CC3DRunConfig

int CC3DRunConfig::parse(Network* network, const char* file)
{ 
    int ret = runConfig->parse(network, file);
    return ret;
}

int CC3DRunConfig::parseExpression(Network* network, const char* expr)
{
    int ret = runConfig->parseExpression(network, expr);
    return ret;
}

CC3DRunConfig::CC3DRunConfig()
{
    runConfig = new RunConfig();
    randGen = new CC3DRandomGenerator();
}

CC3DRunConfig::CC3DRunConfig(const CC3DRunConfig& other) 
{
    runConfig = new RunConfig();
    randGen = new CC3DRandomGenerator();

    setTimeTick(other.getTimeTick());
    setSampleCount(other.getSampleCount());
    setDiscreteTime(other.getDiscreteTime());
    setSeed(other.getSeed());
}

CC3DRunConfig::~CC3DRunConfig()
{
    delete runConfig;
    runConfig = 0;
    delete randGen;
    randGen = 0;
}

RunConfig* CC3DRunConfig::getRunConfig() 
{
    return runConfig;
}

CC3DRandomGenerator* CC3DRunConfig::getRandomGenerator() {
    return randGen;
}

double CC3DRunConfig::getTimeTick() const {
    return runConfig->getTimeTick();
}

void CC3DRunConfig::setTimeTick(const double &time_tick) {
    runConfig->setParameter("time_tick", time_tick);
}

unsigned int CC3DRunConfig::getSampleCount() const {
    return runConfig->getSampleCount();
}

void CC3DRunConfig::setSampleCount(const unsigned int &sample_count) {
    runConfig->setParameter("sample_count", sample_count);
}

const bool CC3DRunConfig::getDiscreteTime() const {
    return runConfig->isDiscreteTime();
}

void CC3DRunConfig::setDiscreteTime(const bool& discrete_time) {
    runConfig->setParameter("discrete_time", (double)discrete_time);
}

int CC3DRunConfig::getSeed() const {
    return randGen->getSeed();
}

void CC3DRunConfig::setSeed(const int &seed) { randGen->setSeed(seed); }

unsigned int CC3DRunConfig::generateUInt32() { 
    return randGen->generateUInt32();
}

double CC3DRunConfig::generate() {
    return randGen->generate();
}

// CC3DMaBoSSNodeAttributeAccessorPy

const Expression* CC3DMaBoSSNodeAttributeAccessorPy::getExpression()
{
    return node->getAttributeExpression(attr_name);
}

void CC3DMaBoSSNodeAttributeAccessorPy::setExpression(const Expression* expr) 
{
    node->setAttributeExpression(attr_name, expr);
}

std::string CC3DMaBoSSNodeAttributeAccessorPy::getString()
{
    return node->getAttributeString(attr_name);
}

void CC3DMaBoSSNodeAttributeAccessorPy::setString(const std::string& str) 
{
    node->setAttributeString(attr_name, str);
}

// CC3DMaBoSSEngine

CC3DMaBoSSEngine::CC3DMaBoSSEngine(const char* ctbndl_file, const char* cfg_file, const double& stepSize, const int& seed, const bool& cfgSeed) :
    time(0.0), 
    stepSize(stepSize), 
    networkState(NetworkState())
{   
    network = new Network();
    network->parse(ctbndl_file);

    runConfig = new CC3DRunConfig();
    runConfig->parse(network, cfg_file);
    if (!cfgSeed) runConfig->setSeed(seed);

    IStateGroup::checkAndComplete(network);
    network->initStates(networkState, runConfig->getRandomGenerator()->getRandomGenerator());
    networkStateInit = networkState;

    engine = new StochasticSimulationEngine(network, runConfig->getRunConfig(), runConfig->getSeed());
}

CC3DMaBoSSEngine::~CC3DMaBoSSEngine()
{
    delete network;
    network = 0;
    delete runConfig;
    runConfig = 0;
    delete engine;
    engine = 0;
}

NodeIndex CC3DMaBoSSEngine::getTargetNode(CC3DRandomGenerator *random_generator, const MAP<NodeIndex, double> &nodeTransitionRates, double total_rate) const
{
    double randomRate = random_generator->generate() * total_rate;
    MAP<NodeIndex, double>::const_iterator itr;
    NodeIndex nodeIdx;
    for (itr = nodeTransitionRates.begin(); itr != nodeTransitionRates.end(); ++itr) {
        nodeIdx = (*itr).first;
        randomRate -= (*itr).second;
        if (randomRate <= 0.) break;
    }

    assert(nodeIdx != INVALID_NODE_INDEX);
    assert(network->getNode(nodeIdx)->getIndex() == nodeIdx);
    return nodeIdx;
}

void CC3DMaBoSSEngine::step(const double& _stepSize)
{
    double simTime = _stepSize > 0. ? _stepSize : stepSize;
    engine->setDiscreteTime(runConfig->getDiscreteTime());
    engine->setTimeTick(runConfig->getTimeTick());
    engine->setMaxTime(simTime);
    networkState = engine->run(networkState);
    time += simTime;
}

void CC3DMaBoSSEngine::loadNetworkState(const NetworkState& _networkState)
{
    const std::vector<Node *>& nodes = network->getNodes();
    for (std::vector<Node *>::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr) {
        Node* node = *itr;
        node->setNodeState(networkState, node->getNodeState(_networkState));
    }
}

Network* CC3DMaBoSSEngine::getNetwork() {
    return network;
}

CC3DRunConfig* CC3DMaBoSSEngine::getRunConfig() {
    return runConfig;
}

NetworkState* CC3DMaBoSSEngine::getNetworkState() {
    return &networkState;
}

CC3DMaBoSSNode CC3DMaBoSSEngine::getNode(const std::string& label) 
{
    return CC3DMaBoSSNode(network->getNode(label), network, &networkState, &networkStateInit, runConfig->getRandomGenerator()->getRandomGenerator());
}

void CC3DMaBoSSEngineContainer::addEngine(CC3DMaBoSSEngine *engine, const long &cellId, const std::string &engineName) {
    storedEngines.push_back(engine);
    enginesByCell[cellId][engineName] = engine;
}

void CC3DMaBoSSEngineContainer::remEngine(const long &cellId, const std::string &engineName) {
    auto itrId = enginesByCell.find(cellId);
    if(itrId != enginesByCell.end()) {
        auto itrName = (*itrId).second.find(engineName);
        if(itrName != (*itrId).second.end()) {
            auto itrEng = std::find(storedEngines.begin(), storedEngines.end(), (*itrName).second);
            if(itrEng != storedEngines.end()) 
                storedEngines.erase(itrEng);
            
            (*itrId).second.erase(itrName);
        }
    }
}

void CC3DMaBoSSEngineContainer::remCell(const long &cellId) {
    auto itrId = enginesByCell.find(cellId);
    if(itrId != enginesByCell.end()) {
        auto cellEngs = (*itrId).second;
        std::vector<std::string> engNames;
        for(auto &itrEng : cellEngs) 
            engNames.push_back(itrEng.first);
        for(auto &eName : engNames) {
            auto cellEngItr = cellEngs.find(eName);
            auto storedEngItr = std::find(storedEngines.begin(), storedEngines.end(), (*cellEngItr).second);
            if(storedEngItr != storedEngines.end()) 
                storedEngines.erase(storedEngItr);
            cellEngs.erase(cellEngItr);
        }
    }
}

void CC3DMaBoSSEngineContainer::step(const double &_stepSize) {
    for(auto &eng : storedEngines) {
        eng->step(_stepSize);
    }
}
