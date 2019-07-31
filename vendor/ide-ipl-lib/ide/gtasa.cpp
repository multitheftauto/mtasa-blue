#include "gtasa.h"

ide::gtasa::tCarObject::tCarObject(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tCarObject::read(ideipl_line& line)
{
    line >> id >> modelName >> txdName >> vehicleType >> handlingID >> gameName >> animName >> className >> frequency >> flags >> comprules >> wheelModelID >>
        wheelScaleFront >> wheelScaleRear >> WheelUpgradeClass;
}

void ide::gtasa::tCarObject::write(ideipl_line& line)
{
    line << id << modelName << txdName << vehicleType << handlingID << gameName << animName << className << frequency << flags << comprules << wheelModelID
         << wheelScaleFront << wheelScaleRear << WheelUpgradeClass;
}

ide::gtasa::tPedObject::tPedObject(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tPedObject::read(ideipl_line& line)
{
    line >> id >> modelName >> txdName >> defaultPedType >> behaviour >> animGroup >> CarsCanDriveMask >> flags >> animFile >> radio1 >> radio2 >>
        voiceArchive >> voice1 >> voice2;
}

void ide::gtasa::tPedObject::write(ideipl_line& line)
{
    line << id << modelName << txdName << defaultPedType << behaviour << animGroup << CarsCanDriveMask << flags << animFile << radio1 << radio2 << voiceArchive
         << voice1 << voice2;
}

ide::gtasa::tObject::tObject()
{
}

ide::gtasa::tObject::tObject(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tObject::read(ideipl_line& line)
{
    parameterCount = line.param_count();

    if (line.param_count() == 6)
        line >> id >> modelName >> txdName >> meshCount >> drawDistance >> flags.int_value;
    else if (line.param_count() == 7)
        line >> id >> modelName >> txdName >> meshCount >> drawDistance >> ideipl_line::skip >> flags.int_value;
    else if (line.param_count() == 8)
        line >> id >> modelName >> txdName >> meshCount >> drawDistance >> ideipl_line::skip >> ideipl_line::skip >> flags.int_value;
    else
        line >> id >> modelName >> txdName >> drawDistance >> flags.int_value;
}

void ide::gtasa::tObject::write(ideipl_line& line)
{
    if (parameterCount == 6)
        line << id << modelName << txdName << meshCount << drawDistance << flags.int_value;
    else if (parameterCount == 7)
        line << id << modelName << txdName << meshCount << drawDistance << ideipl_line::skip << flags.int_value;
    else if (parameterCount == 8)
        line << id << modelName << txdName << meshCount << drawDistance << ideipl_line::skip << ideipl_line::skip << flags.int_value;
    else
        line << id << modelName << txdName << drawDistance << flags.int_value;
}

ide::gtasa::tTimeObject::tTimeObject(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tTimeObject::read(ideipl_line& line)
{
    parameterCount = line.param_count();
    if (line.param_count() == 8)
        line >> id >> modelName >> txdName >> meshCount >> drawDistance >> flags.int_value >> timeOn >> timeOff;
    else if (line.param_count() == 9)
        line >> id >> modelName >> txdName >> meshCount >> drawDistance >> ideipl_line::skip >> flags.int_value >> timeOn >> timeOff;
    else if (line.param_count() == 11)
        line >> id >> modelName >> txdName >> meshCount >> drawDistance >> ideipl_line::skip >> ideipl_line::skip >> flags.int_value >> timeOn >>
            timeOff;
    else
        line >> id >> modelName >> txdName >> drawDistance >> flags.int_value >> timeOn >> timeOff;
}

void ide::gtasa::tTimeObject::write(ideipl_line& line)
{
    if (parameterCount == 8)
        line << id << modelName << txdName << meshCount << drawDistance << flags.int_value << timeOn << timeOff;
    else if (parameterCount == 9)
        line << id << modelName << txdName << meshCount << drawDistance << ideipl_line::skip << flags.int_value << timeOn << timeOff;
    else if (parameterCount == 11)
        line << id << modelName << txdName << meshCount << drawDistance << ideipl_line::skip << ideipl_line::skip << flags.int_value << timeOn << timeOff;
    else
        line << id << modelName << txdName << drawDistance << flags.int_value << timeOn << timeOff;
}

ide::gtasa::tHierObject::tHierObject(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tHierObject::read(ideipl_line& line)
{
    line >> id >> modelName >> txdName;
}

void ide::gtasa::tHierObject::write(ideipl_line& line)
{
    line << id << modelName << txdName;
}

ide::gtasa::tAnimObject::tAnimObject()
{
}

ide::gtasa::tAnimObject::tAnimObject(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tAnimObject::read(ideipl_line& line)
{
    line >> id >> modelName >> txdName >> animName >> drawDistance >> flags.int_value;
}

void ide::gtasa::tAnimObject::write(ideipl_line& line)
{
    line << id << modelName << txdName << animName << drawDistance << flags.int_value;
}

ide::gtasa::tWeapObject::tWeapObject(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tWeapObject::read(ideipl_line& line)
{
    line >> id >> modelName >> txdName >> animName >> ideipl_line::skip >> drawDistance;
}

void ide::gtasa::tWeapObject::write(ideipl_line& line)
{
    line << id << modelName << txdName << animName << 1 << drawDistance;
}

ide::gtasa::tTxdParent::tTxdParent(ideipl_line& line)
{
    read(line);
}

void ide::gtasa::tTxdParent::read(ideipl_line& line)
{
    line >> txdName >> parentTxdName;
}

void ide::gtasa::tTxdParent::write(ideipl_line& line)
{
    line << txdName << parentTxdName;
}

bool ide::gtasa::ide_file::LoadIDE()
{
    bool          bFileOpenSuccess = false;
    std::ifstream stream(m_filePath);
    item_type     item = item_na;
    if (stream.is_open())
    {
        bFileOpenSuccess = true;
        for (std::string fileline; getline(stream, fileline);)
        {
            if (!fileline.empty() && fileline[0] != '#' && fileline.find_first_not_of(" \t") != std::string::npos)
            {
                if (item == item_na)
                {
                    if (!fileline.compare(0, 4, "objs"))
                        item = item_objs;
                    else if (!fileline.compare(0, 4, "tobj"))
                        item = item_tobj;
                    else if (!fileline.compare(0, 4, "hier"))
                        item = item_hier;
                    else if (!fileline.compare(0, 4, "anim"))
                        item = item_anim;
                    else if (!fileline.compare(0, 4, "weap"))
                        item = item_weap;
                    else if (!fileline.compare(0, 4, "txdp"))
                        item = item_txdp;
                    else if (!fileline.compare(0, 4, "cars"))
                        item = item_cars;
                    else if (!fileline.compare(0, 4, "peds"))
                        item = item_peds;
                }
                else
                {
                    if (!fileline.compare(0, 3, "end"))
                        item = item_na;
                    else
                    {
                        ideipl_line line(fileline);
                        if (item == item_objs)
                            objs.emplace_back(line);
                        else if (item == item_tobj)
                            tobj.emplace_back(line);
                        else if (item == item_hier)
                            hier.emplace_back(line);
                        else if (item == item_anim)
                            anim.emplace_back(line);
                        else if (item == item_weap)
                            weap.emplace_back(line);
                        else if (item == item_txdp)
                            txdp.emplace_back(line);
                        else if (item == item_cars)
                            cars.emplace_back(line);
                        else if (item == item_peds)
                            peds.emplace_back(line);
                    }
                }
            }
        }
    }

    return bFileOpenSuccess;
}

std::string getFileName(std::string filePath, bool withExtension = true, char seperator = '\\')
{
    // Get last dot position
    std::size_t dotPos = filePath.rfind('.');
    std::size_t sepPos = filePath.rfind(seperator);

    if (sepPos != std::string::npos)
    {
        return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos));
    }
    return "";
}

// DO NOT PASS THE .IDE name, only the FOLDER PATH
// program will do folderLocation + "\\" + ideNameWithExtension
bool ide::gtasa::ide_file::WriteIDE(const std::string& folderLocation)
{
    std::string   ideNameWithExtension = getFileName(m_filePath);
    std::ofstream outputFileStream(folderLocation + "\\" + ideNameWithExtension, std::ofstream::out | std::ios::trunc);
    if (outputFileStream.fail())
    {
        printf("ide::gtasa::ide_file::WriteIDE failed for: %s\\%s\n", folderLocation.c_str(), ideNameWithExtension.c_str());
        return false;
    }


    WriteObjsObject(outputFileStream);
    WriteTimeObjects(outputFileStream);
    WriteHierObjects(outputFileStream);
    WriteAnimObjects(outputFileStream);
    WriteWeaponObjects(outputFileStream);
    WriteTxdParentObjects(outputFileStream);
    WriteCarObjects(outputFileStream); 
    WritePedObjects(outputFileStream);
    outputFileStream.close();
    return true;
}

void ide::gtasa::ide_file::WriteObjsObject(std::ofstream& outputFileStream)
{
    outputFileStream << "objs\n";
    for (auto& obj : objs)
    {
        ideipl_line line(obj.parameterCount);
        obj.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}

void ide::gtasa::ide_file::WriteTimeObjects(std::ofstream& outputFileStream)
{
    outputFileStream << "tobj\n";
    for (auto& timeObject : tobj)
    {
        ideipl_line line(timeObject.parameterCount);
        timeObject.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}

void ide::gtasa::ide_file::WriteHierObjects(std::ofstream& outputFileStream)
{
    outputFileStream << "hier\n";
    for (auto& hierObject : hier)
    {
        ideipl_line line(hierObject.parameterCount);
        hierObject.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}

void ide::gtasa::ide_file::WriteAnimObjects(std::ofstream& outputFileStream)
{
    outputFileStream << "anim\n";
    for (auto& animObject : anim)
    {
        ideipl_line line(animObject.parameterCount);
        animObject.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}

void ide::gtasa::ide_file::WriteWeaponObjects(std::ofstream& outputFileStream)
{
    outputFileStream << "weap\n";
    for (auto& weapObject : weap)
    {
        ideipl_line line(weapObject.parameterCount);
        weapObject.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}

void ide::gtasa::ide_file::WriteTxdParentObjects(std::ofstream& outputFileStream)
{
    outputFileStream << "txdp\n";
    for (auto& txdpObject : txdp)
    {
        ideipl_line line(txdpObject.parameterCount);
        txdpObject.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}

void ide::gtasa::ide_file::WriteCarObjects(std::ofstream& outputFileStream)
{
    outputFileStream << "cars\n";
    for (auto& carObject : cars)
    {
        ideipl_line line(carObject.parameterCount);
        carObject.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}

void ide::gtasa::ide_file::WritePedObjects(std::ofstream& outputFileStream)
{
    outputFileStream << "peds\n";
    for (auto& pedObject : peds)
    {
        ideipl_line line(pedObject.parameterCount);
        pedObject.write(line);
        std::string outputString;
        line.GetEntriesAsConcatenatedLine(outputString);
        outputFileStream << outputString << "\n";
    }
    outputFileStream << "end\n";
}
