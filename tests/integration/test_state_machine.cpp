#include <gtest/gtest.h>
#include "trlc/enum.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <set>

namespace {

// State machine for a document workflow with transitions
TRLC_ENUM(DocumentState,
    Draft = TRLC_FIELD(value = 0, desc = "Document is being drafted", extra = "EDITABLE"),
    Review = TRLC_FIELD(value = 1, desc = "Document is under review", extra = "READONLY"),
    Approved = TRLC_FIELD(value = 2, desc = "Document has been approved", extra = "READONLY"),
    Published = TRLC_FIELD(value = 3, desc = "Document is published", extra = "READONLY"),
    Archived = TRLC_FIELD(value = 4, desc = "Document is archived", extra = "READONLY"),
    Rejected = TRLC_FIELD(value = 5, desc = "Document was rejected", extra = "EDITABLE")
);

// Helper class to manage state transitions
class StateMachine {
public:
    explicit StateMachine(DocumentState::Enum initial_state) 
        : current_state_(initial_state) {
        init_transitions();
    }
    
    bool can_transition_to(DocumentState::Enum target) const {
        auto it = allowed_transitions_.find(current_state_.index());
        if (it == allowed_transitions_.end()) {
            return false;
        }
        return it->second.count(target.index()) > 0;
    }
    
    bool transition_to(DocumentState::Enum target) {
        if (!can_transition_to(target)) {
            return false;
        }
        
        history_.push_back(current_state_);
        current_state_ = target;
        return true;
    }
    
    DocumentState::Enum current_state() const {
        return current_state_;
    }
    
    std::vector<DocumentState::Enum> history() const {
        return history_;
    }
    
    bool is_editable() const {
        return current_state_.extra() == "EDITABLE";
    }
    
    std::vector<DocumentState::Enum> get_allowed_transitions() const {
        std::vector<DocumentState::Enum> result;
        auto it = allowed_transitions_.find(current_state_.index());
        if (it != allowed_transitions_.end()) {
            for (std::size_t idx : it->second) {
                result.push_back(DocumentState::Enum{idx});
            }
        }
        return result;
    }
    
private:
    void init_transitions() {
        // Define allowed state transitions using indices
        allowed_transitions_[DocumentState::Draft.index()] = {
            DocumentState::Review.index()
        };
        
        allowed_transitions_[DocumentState::Review.index()] = {
            DocumentState::Draft.index(),     // Send back for revision
            DocumentState::Approved.index(),
            DocumentState::Rejected.index()
        };
        
        allowed_transitions_[DocumentState::Approved.index()] = {
            DocumentState::Published.index()
        };
        
        allowed_transitions_[DocumentState::Published.index()] = {
            DocumentState::Archived.index()
        };
        
        allowed_transitions_[DocumentState::Rejected.index()] = {
            DocumentState::Draft.index()      // Can restart from draft
        };
        
        // Archived is terminal - no transitions allowed
    }
    
    DocumentState::Enum current_state_;
    std::vector<DocumentState::Enum> history_;
    std::map<std::size_t, std::set<std::size_t>> allowed_transitions_;
};

TEST(StateMachineTest, InitialState) {
    StateMachine sm(DocumentState::Draft);
    EXPECT_EQ(sm.current_state(), DocumentState::Draft);
    EXPECT_TRUE(sm.history().empty());
}

TEST(StateMachineTest, ValidTransition) {
    StateMachine sm(DocumentState::Draft);
    EXPECT_TRUE(sm.transition_to(DocumentState::Review));
    EXPECT_EQ(sm.current_state(), DocumentState::Review);
}

TEST(StateMachineTest, InvalidTransition) {
    StateMachine sm(DocumentState::Draft);
    EXPECT_FALSE(sm.transition_to(DocumentState::Published));
    EXPECT_EQ(sm.current_state(), DocumentState::Draft);
}

TEST(StateMachineTest, FullWorkflow) {
    StateMachine sm(DocumentState::Draft);
    
    // Draft -> Review
    EXPECT_TRUE(sm.transition_to(DocumentState::Review));
    EXPECT_EQ(sm.current_state(), DocumentState::Review);
    
    // Review -> Approved
    EXPECT_TRUE(sm.transition_to(DocumentState::Approved));
    EXPECT_EQ(sm.current_state(), DocumentState::Approved);
    
    // Approved -> Published
    EXPECT_TRUE(sm.transition_to(DocumentState::Published));
    EXPECT_EQ(sm.current_state(), DocumentState::Published);
    
    // Published -> Archived
    EXPECT_TRUE(sm.transition_to(DocumentState::Archived));
    EXPECT_EQ(sm.current_state(), DocumentState::Archived);
}

TEST(StateMachineTest, RejectionWorkflow) {
    StateMachine sm(DocumentState::Draft);
    
    // Draft -> Review
    EXPECT_TRUE(sm.transition_to(DocumentState::Review));
    
    // Review -> Rejected
    EXPECT_TRUE(sm.transition_to(DocumentState::Rejected));
    EXPECT_EQ(sm.current_state(), DocumentState::Rejected);
    
    // Rejected -> Draft (restart)
    EXPECT_TRUE(sm.transition_to(DocumentState::Draft));
    EXPECT_EQ(sm.current_state(), DocumentState::Draft);
}

TEST(StateMachineTest, RevisionWorkflow) {
    StateMachine sm(DocumentState::Draft);
    
    // Draft -> Review
    EXPECT_TRUE(sm.transition_to(DocumentState::Review));
    
    // Review -> Draft (send back for revision)
    EXPECT_TRUE(sm.transition_to(DocumentState::Draft));
    EXPECT_EQ(sm.current_state(), DocumentState::Draft);
}

TEST(StateMachineTest, TransitionHistory) {
    StateMachine sm(DocumentState::Draft);
    
    sm.transition_to(DocumentState::Review);
    sm.transition_to(DocumentState::Approved);
    sm.transition_to(DocumentState::Published);
    
    const auto& history = sm.history();
    EXPECT_EQ(history.size(), 3);
    EXPECT_EQ(history[0], DocumentState::Draft);
    EXPECT_EQ(history[1], DocumentState::Review);
    EXPECT_EQ(history[2], DocumentState::Approved);
}

TEST(StateMachineTest, EditableStates) {
    EXPECT_TRUE(StateMachine(DocumentState::Draft).is_editable());
    EXPECT_TRUE(StateMachine(DocumentState::Rejected).is_editable());
    
    EXPECT_FALSE(StateMachine(DocumentState::Review).is_editable());
    EXPECT_FALSE(StateMachine(DocumentState::Approved).is_editable());
    EXPECT_FALSE(StateMachine(DocumentState::Published).is_editable());
    EXPECT_FALSE(StateMachine(DocumentState::Archived).is_editable());
}

TEST(StateMachineTest, TerminalState) {
    StateMachine sm(DocumentState::Draft);
    
    // Navigate to archived state
    sm.transition_to(DocumentState::Review);
    sm.transition_to(DocumentState::Approved);
    sm.transition_to(DocumentState::Published);
    sm.transition_to(DocumentState::Archived);
    
    // Cannot transition from archived
    EXPECT_FALSE(sm.can_transition_to(DocumentState::Draft));
    EXPECT_FALSE(sm.can_transition_to(DocumentState::Review));
    EXPECT_FALSE(sm.can_transition_to(DocumentState::Published));
}

TEST(StateMachineTest, AllowedTransitions) {
    StateMachine sm(DocumentState::Review);
    
    auto allowed = sm.get_allowed_transitions();
    EXPECT_EQ(allowed.size(), 3);
    
    // Review can go to Draft, Approved, or Rejected
    EXPECT_TRUE(std::any_of(allowed.begin(), allowed.end(), [](auto s) {
        return s == DocumentState::Draft;
    }));
    EXPECT_TRUE(std::any_of(allowed.begin(), allowed.end(), [](auto s) {
        return s == DocumentState::Approved;
    }));
    EXPECT_TRUE(std::any_of(allowed.begin(), allowed.end(), [](auto s) {
        return s == DocumentState::Rejected;
    }));
}

TEST(StateMachineTest, StateDescriptions) {
    EXPECT_EQ(DocumentState::Draft.desc(), "Document is being drafted");
    EXPECT_EQ(DocumentState::Review.desc(), "Document is under review");
    EXPECT_EQ(DocumentState::Approved.desc(), "Document has been approved");
}

TEST(StateMachineTest, StateValues) {
    EXPECT_EQ(DocumentState::Draft.value(), 0);
    EXPECT_EQ(DocumentState::Review.value(), 1);
    EXPECT_EQ(DocumentState::Approved.value(), 2);
    EXPECT_EQ(DocumentState::Published.value(), 3);
    EXPECT_EQ(DocumentState::Archived.value(), 4);
    EXPECT_EQ(DocumentState::Rejected.value(), 5);
}

TEST(StateMachineTest, LookupStateByValue) {
    auto state = DocumentState::fromValue(2);
    ASSERT_TRUE(state.has_value());
    EXPECT_EQ(state->name(), "Approved");
    EXPECT_EQ(state->desc(), "Document has been approved");
}

TEST(StateMachineTest, IterateAllStates) {
    std::size_t count = 0;
    
    for (auto state : DocumentState::all()) {
        EXPECT_FALSE(state.name().empty());
        EXPECT_FALSE(state.desc().empty());
        EXPECT_FALSE(state.extra().empty());
        count++;
    }
    
    EXPECT_EQ(count, DocumentState::SIZE);
}

TEST(StateMachineTest, CompareStates) {
    EXPECT_TRUE(DocumentState::Draft == DocumentState::Draft);
    EXPECT_FALSE(DocumentState::Draft == DocumentState::Review);
    EXPECT_TRUE(DocumentState::Draft != DocumentState::Review);
}

TEST(StateMachineTest, MultipleInstances) {
    StateMachine sm1(DocumentState::Draft);
    StateMachine sm2(DocumentState::Review);
    
    sm1.transition_to(DocumentState::Review);
    sm2.transition_to(DocumentState::Approved);
    
    EXPECT_EQ(sm1.current_state(), DocumentState::Review);
    EXPECT_EQ(sm2.current_state(), DocumentState::Approved);
}

TEST(StateMachineTest, InvalidTransitionPreservesState) {
    StateMachine sm(DocumentState::Draft);
    
    EXPECT_FALSE(sm.transition_to(DocumentState::Archived));
    EXPECT_EQ(sm.current_state(), DocumentState::Draft);
    EXPECT_TRUE(sm.history().empty());
}

TEST(StateMachineTest, CanTransitionCheck) {
    StateMachine sm(DocumentState::Draft);
    
    EXPECT_TRUE(sm.can_transition_to(DocumentState::Review));
    EXPECT_FALSE(sm.can_transition_to(DocumentState::Approved));
    EXPECT_FALSE(sm.can_transition_to(DocumentState::Published));
    EXPECT_FALSE(sm.can_transition_to(DocumentState::Archived));
    EXPECT_FALSE(sm.can_transition_to(DocumentState::Rejected));
}

} // namespace
