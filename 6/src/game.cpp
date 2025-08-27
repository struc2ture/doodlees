#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>

#include <imgui.h>

#include "types.hpp"

#include "gl_tiles.cpp"
#include "util.hpp"

// Ontological ring
struct Payload
{
    enum class Kind : int
    {
        NONE,
        Love,
        Death,
        Salt,
        WillowBranches,
        Asparagus,
        Spinach,
        WatermelonSlices,
        Moon,
        Dream,
        Longing,
        Notebook,
        SealedLetter,
        Heaven,
        Sun,
        Earth,
        Employment,
        Logos,
        Location,
        Kairos,
        COUNT
    };

    Kind kind;

    Payload(Kind kind)
    {
        this->kind = kind;
    }

    static const char *get_kind_string(Kind kind)
    {
        switch (kind)
        {
            case Kind::NONE: return "NONE";
            case Kind::Love: return "Love";
            case Kind::Death: return "Death";
            case Kind::Salt: return "Salt";
            case Kind::WillowBranches: return "Willow Branches";
            case Kind::Asparagus: return "Asparagus";
            case Kind::Spinach: return "Spinach";
            case Kind::WatermelonSlices: return "Watermelon Slices";
            case Kind::Moon: return "Moon";
            case Kind::Dream: return "Dream";
            case Kind::Longing: return "Longing";
            case Kind::Notebook: return "Notebook";
            case Kind::SealedLetter: return "Sealed Letter";
            case Kind::Heaven: return "Heaven";
            case Kind::Sun: return "Sun";
            case Kind::Earth: return "Earth";
            case Kind::Employment: return "Employment";
            case Kind::Logos: return "Logos";
            case Kind::Location: return "Location";
            case Kind::Kairos: return "Kairos";
            case Kind::COUNT: return "UNKNOWN";
        }
    }

    const char *get_kind_string()
    {
        return get_kind_string(kind);
    }

    static Kind get_random_kind()
    {
        Kind kind = (Kind)(rand() % (int)Kind::COUNT);
        return kind;
    }
};

struct Node
{
    enum class Kind
    {
        NONE,
        Storage,
        Transmuter,
        COUNT
    };


    char name_buf[STR_BUF_SMALL];
    Kind kind;

    bool is_window_open = false;

    std::map<Payload::Kind, int> input_buffer;
    std::map<Payload::Kind, int> output_buffer;

    float progress = 0.0f;
    float rate = 0.6f;

    Node(const char *name, Kind kind)
    {
        this->kind = kind;
        strcpy(this->name_buf, name);
    }

    Node(const char *name, Kind kind, int total_payload_count) : Node(name, kind)
    {
        while (get_output_payload_count() < total_payload_count)
        {
            Payload::Kind random_kind = (Payload::Kind)(rand() % (int)Payload::Kind::COUNT);
            add_payload_to_output_buffer(random_kind, 1);
        }
    }

    static const char *get_kind_str(Kind kind)
    {
        switch (kind)
        {
            case Kind::NONE: return "NONE";
            case Kind::Storage: return "Storage";
            case Kind::Transmuter: return "Transmuter";
            default: return "UNKNOWN";
        }
    }

    const char *get_kind_str()
    {
        return get_kind_str(kind);
    }

    static const char *get_random_name()
    {
        const char *names[] =
        {
            "Beta",
            "Gamma",
            "Delta",
            "Epsilon",
            "Zeta",
            "Eta",
        };
        int i = rand() % array_size(names);
        return names[i];
    }

    void add_payload_to_output_buffer(Payload::Kind kind, int count)
    {
        output_buffer[kind] += count;
    }

    void remove_payload_from_output_buffer(Payload::Kind kind, int count)
    {
        if (output_buffer[kind] > 0)
        {
            output_buffer[kind] -= count;
        }
    }

    void add_payload_to_input_buffer(Payload::Kind kind, int count)
    {
        input_buffer[kind] += count;
    }

    void remove_payload_from_input_buffer(Payload::Kind kind, int count)
    {
        if (input_buffer[kind] > 0)
        {
            input_buffer[kind] -= count;
        }
    }

    inline int get_output_payload_count()
    {
        int total_count = 0;
        for (auto it = output_buffer.begin(); it != output_buffer.end(); it++)
        {
            if (it->first != Payload::Kind::NONE && it->first != Payload::Kind::COUNT)
            {
                total_count += it->second;
            }
        }
        return total_count;
    }

    Payload::Kind retrieve_random_output_payload()
    {
        Payload::Kind kind = Payload::Kind::NONE;

        std::vector<Payload::Kind> available_kinds;
        for (auto it = output_buffer.begin(); it != output_buffer.end(); it++)
        {
            if (it->second > 0) available_kinds.push_back(it->first);
        }

        if (available_kinds.size() > 0)
        {
            kind = available_kinds[rand() % available_kinds.size()];
            remove_payload_from_output_buffer(kind, 1);
        }

        return kind;
    }

    void update_progress(float delta)
    {
        if (kind == Kind::Transmuter)
        {
            progress += delta * rate;
        }

        if (progress > 1.0f)
        {
            for (auto it = input_buffer.begin(); it != input_buffer.end(); it++)
            {
                int count = it->second;
                remove_payload_from_input_buffer(it->first, count);
                Payload::Kind new_kind = (Payload::Kind)((int)it->first + 1);
                if (new_kind >= Payload::Kind::COUNT) new_kind = (Payload::Kind)1;
                add_payload_to_output_buffer(new_kind, count);
            }
            progress = 0.0f;
        }
    }
};

struct Agent
{
    char name_buf[STR_BUF_SMALL];
    bool is_window_open = false;
    size_t node_a = 0;
    size_t node_b = 0;
    float progress = 0.0f;
    float progress_rate = 0.3f;
    bool travelling_from_b = false;
    Payload carried_payload{Payload::Kind::NONE};

    Agent(const char *name)
    {
        strcpy(this->name_buf, name);
    }

    inline bool destinations_valid()
    {
        return node_a > 0 && node_b > 0 && node_a != node_b;
    }

    void start_delivery(std::vector<Node> &nodes)
    {
        size_t which_node = travelling_from_b ? node_b : node_a;

        // trace("Starting delivery from %s to %s.", travelling_from_b ? "B" : "A", travelling_from_b ? "A" : "B");

        if (nodes[which_node].get_output_payload_count() > 0)
        {
            carried_payload = {nodes[which_node].retrieve_random_output_payload()};
        }
        else
        {
            carried_payload = {Payload::Kind::NONE};
        }
    }

    void finish_delivery(std::vector<Node> &nodes)
    {
        if (carried_payload.kind != Payload::Kind::NONE)
        {
            size_t node_index;
            if (!travelling_from_b) node_index = node_b;
            else node_index = node_a;
            nodes[node_index].add_payload_to_input_buffer(carried_payload.kind, 1);
        }
        travelling_from_b = !travelling_from_b;
        progress = 0.0f;
    }

    void update_progress(std::vector<Node> &nodes, float delta)
    {
        if (destinations_valid())
        {
            if (progress <= 0.0f)
            {
                start_delivery(nodes);
            }

            if (carried_payload.kind != Payload::Kind::NONE)
            {
                progress += delta * progress_rate;
                if (progress > 1.0f)
                {
                    finish_delivery(nodes);
                }
            }
        }
    }

    static const char *get_random_name()
    {
        const char *random_names[] =
        {
            "Humgef",
            "Haliser",
            "Kierty",
            "Giolist",
            "Leemper"
        };
        int i = rand() % array_size(random_names);
        return random_names[i];
    }
};

struct Game
{
    std::vector<Agent> agents;
    std::vector<Node> nodes;

    char agent_list_name_edit_buf[STR_BUF_SMALL];
    char node_list_name_edit_buf[STR_BUF_SMALL];

    bool alpha_node_exists = false;

    void init()
    {
        srand(0);
        agents.push_back(Agent("NONE"));
        nodes.push_back(Node("NONE", Node::Kind::NONE));
        strcpy(agent_list_name_edit_buf, Agent::get_random_name());
        strcpy(node_list_name_edit_buf, Node::get_random_name());
    }

    void frame(float delta)
    {
        for (size_t i = 1; i < agents.size(); i++)
        {
            agents[i].update_progress(nodes, delta);
        }

        for (size_t i = 1; i < nodes.size(); i++)
        {
            nodes[i].update_progress(delta);
        }

        draw_agent_list_window();

        draw_agent_windows();

        draw_node_list_window();

        draw_node_windows();
    }

    void draw_agent_list_window()
    {
        ImGui::Begin("Agents");

        ImGui::InputText("Name", agent_list_name_edit_buf, sizeof(agent_list_name_edit_buf));
        ImGui::SameLine();
        if (ImGui::Button("Add"))
        {
            agents.push_back(Agent(agent_list_name_edit_buf));
            strcpy(agent_list_name_edit_buf, Agent::get_random_name());
        }

        char item_name_buf[STR_BUF_SMALL];
        for (size_t i = 1; i < agents.size(); i++)
        {
            ImGui::PushID(agents.data() + i);
            ImGui::Bullet();
            snprintf(item_name_buf, sizeof(item_name_buf), "Agent %s", agents[i].name_buf);
            if (ImGui::TextLink(item_name_buf))
            {
                agents[i].is_window_open = !agents[i].is_window_open;
                trace("%zu: window open = %d", i, agents[i].is_window_open);
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    void draw_agent_windows()
    {
        for (size_t i = 1; i < agents.size(); i++)
        {
            if (agents[i].is_window_open)
            {
                ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

                char window_name_buf[STR_BUF_SMALL];
                snprintf(window_name_buf, sizeof(window_name_buf), "Agent: %s###Agent%zu", agents[i].name_buf, i);

                if (ImGui::Begin(window_name_buf, &agents[i].is_window_open))
                {
                    ImGui::InputText("Name", agents[i].name_buf, sizeof(agents[i].name_buf));

                    if (ImGui::BeginCombo("Node A", nodes[agents[i].node_a].name_buf, 0))
                    {
                        for (size_t node_i = 0; node_i < nodes.size(); node_i++)
                        {
                            const bool is_selected = agents[i].node_a == node_i;
                            if (ImGui::Selectable(nodes[node_i].name_buf, is_selected))
                            {
                                agents[i].node_a = node_i;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (ImGui::BeginCombo("Node B", nodes[agents[i].node_b].name_buf, 0))
                    {
                        for (size_t node_i = 0; node_i < nodes.size(); node_i++)
                        {
                            const bool is_selected = agents[i].node_b == node_i;
                            if (ImGui::Selectable(nodes[node_i].name_buf, is_selected))
                            {
                                agents[i].node_b = node_i;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (agents[i].destinations_valid())
                    {
                        ImGui::BulletText("Travel direction: %s", agents[i].travelling_from_b ? "B -> A" : "A -> B");
                        ImGui::BulletText("Carried payload: %s", agents[i].carried_payload.get_kind_string());
                        ImGui::BulletText("Progress: %.3f", agents[i].progress);
                    }

                    ImGui::End();
                }
            }
        }
    }

    void draw_node_list_window()
    {
        ImGui::Begin("Nodes");

        ImGui::BeginDisabled(alpha_node_exists);
        if (ImGui::Button("Add alpha"))
        {
            alpha_node_exists = true;
            nodes.push_back(Node("Alpha", Node::Kind::Storage, 10));
        }
        ImGui::EndDisabled();

        ImGui::InputText("Name", node_list_name_edit_buf, sizeof(node_list_name_edit_buf));
        ImGui::SameLine();
        if (ImGui::Button("Add transmuter"))
        {
            nodes.push_back(Node(node_list_name_edit_buf, Node::Kind::Transmuter));
            strcpy(node_list_name_edit_buf, Node::get_random_name());
        }

        char item_name_buf[STR_BUF_SMALL];
        for (size_t i = 1; i < nodes.size(); i++)
        {
            ImGui::PushID(nodes.data() + i);
            ImGui::Bullet();
            snprintf(item_name_buf, sizeof(item_name_buf), "Node %s", nodes[i].name_buf);
            if (ImGui::TextLink(item_name_buf))
            {
                nodes[i].is_window_open = !nodes[i].is_window_open;
                trace("%zu: window open = %d", i, nodes[i].is_window_open);
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    void draw_node_windows()
    {
        for (size_t node_i = 1; node_i < nodes.size(); node_i++)
        {
            if (nodes[node_i].is_window_open)
            {
                ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

                char window_name_buf[STR_BUF_SMALL];
                snprintf(window_name_buf, sizeof(window_name_buf), "Node: %s###Node%zu", nodes[node_i].name_buf, node_i);

                if (ImGui::Begin(window_name_buf, &nodes[node_i].is_window_open))
                {
                    ImGui::InputText("Name", nodes[node_i].name_buf, sizeof(nodes[node_i].name_buf));

                    ImGui::BulletText("Kind: %s", nodes[node_i].get_kind_str());

                    ImGui::BulletText("Progress: %.3f", nodes[node_i].progress);

                    ImGui::Text("Input buffer:");
                    for (auto it = nodes[node_i].input_buffer.begin(); it != nodes[node_i].input_buffer.end(); it++)
                    {
                        if (it->first != Payload::Kind::NONE && it->first != Payload::Kind::COUNT && it->second > 0)
                        {
                            ImGui::BulletText("%d x %s", it->second, Payload::get_kind_string(it->first));
                        }
                    }

                    ImGui::Text("Output buffer:");
                    for (auto it = nodes[node_i].output_buffer.begin(); it != nodes[node_i].output_buffer.end(); it++)
                    {
                        if (it->first != Payload::Kind::NONE && it->first != Payload::Kind::COUNT && it->second > 0)
                        {
                            ImGui::BulletText("%d x %s", it->second, Payload::get_kind_string(it->first));
                        }
                    }

                    ImGui::End();
                }
            }
        }
    }
};
