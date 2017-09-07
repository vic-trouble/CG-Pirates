#include <gtest/gtest.h>

#include "commands.h"
#include "game.h"
#include "io.h"

#include <sstream>

namespace test
{
    template <typename Command>
    testing::AssertionResult isCommand(const std::unique_ptr<game::ICommand> &command)
    {
        if (dynamic_cast<const Command *>(command.get()))
            return testing::AssertionSuccess();
        else
            return testing::AssertionFailure() << "command is " << (command ? typeid(*command.get()).name() : "null");
    }
    
    inline testing::AssertionResult isFireCommand(const hex::Pos &target, const std::unique_ptr<game::ICommand> &command)
    {
        if (const game::Fire *fireCommand = dynamic_cast<const game::Fire *>(command.get()))
        {
            if (fireCommand->pos_ == target)
                return testing::AssertionSuccess();
            else
            {
                std::stringstream ss;
                ss << "Fire command has wrong target " << fireCommand->pos_;
                return testing::AssertionFailure() << ss.str();
            }
        }
        else
            return testing::AssertionFailure() << "command is " << (command ? typeid(*command.get()).name() : "null");
    }
}
