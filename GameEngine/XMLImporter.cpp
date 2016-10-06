#include "XMLImporter.h"
#include "Log.h"

XMLImporter::XMLImporter(void)
{
}

XMLImporter::~XMLImporter(void)
{
}

void XMLImporter::openXMLFile(const char* filename)
{
	mXMLFile.open(filename);

	if(mXMLFile.fail())
	{
		//report error and early out
		Log::Error("Could not open XML file.", "XMLImporter", this);
		return;
	}

	buffer = std::vector<char> ((std::istreambuf_iterator<char>(mXMLFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	mXMLDocument.parse<0>(&buffer[0]);
}

void XMLImporter::Shutdown()
{
	if(mXMLFile.is_open())
		mXMLFile.close();

	mXMLDocument.clear();

	buffer.empty();
}

void XMLImporter::setRootNode(std::string node)
{
	mRootNode = mXMLDocument.first_node(node.c_str());
}

xmlnode* XMLImporter::getNode(std::string node)
{
	mNode = mRootNode->first_node(node.c_str());
	return mNode;
}

xmlnode* XMLImporter::getRoot()
{
	return mRootNode;
}

const char* XMLImporter::getValueOfNode(std::string value) const
{
	return mNode->first_attribute(value.c_str())->value();
}