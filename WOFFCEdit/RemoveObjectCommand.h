#pragma once
#include <vector>

#include "SceneCommand.h"

class DisplayObject;
class RemoveObjectCommand : public SceneCommand
{
public:
	RemoveObjectCommand(std::vector<int> ids);
	void Execute(ToolMain* tool) override;
	void ExecuteSilent(ToolMain* tool) override;
	void Revert(ToolMain* tool) override;
private:
	std::vector<int> m_objectIds;
	std::vector<DisplayObject*> m_subjects;

};

