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

struct Payload
{
    // Ontological ring
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

    static inline Payload NONE()
    {
        return Payload{Kind::NONE};
    }

    inline bool is_none()
    {
        return kind == Kind::NONE;
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
        int random_index = (rand() % ((int)Kind::COUNT - 1)) + 1;
        return (Kind)random_index;
    }

    void transmute()
    {
        kind = (Payload::Kind)((int)kind + 1);
        if (kind >= Payload::Kind::COUNT)
        {
            kind = (Payload::Kind)1;
        }
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

    std::vector<Payload> input_buffer;
    std::vector<float> progress;
    std::vector<Payload> output_buffer;

    float rate = 0.6f;

    Node(const char *name, Kind kind)
    {
        this->kind = kind;
        strcpy(this->name_buf, name);
    }

    Node(const char *name, Kind kind, int random_payload_count) : Node(name, kind)
    {
        while ((int)output_buffer.size() < random_payload_count)
        {
            add_payload_to_output_buffer(Payload::get_random_kind());
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

    void add_payload_to_output_buffer(Payload payload)
    {
        output_buffer.push_back(payload);
    }

    void add_payload_to_input_buffer(Payload payload)
    {
        input_buffer.push_back(payload);
        progress.push_back(0.0f);
    }

    void move_payload_from_input_to_output(int index)
    {
    }

    Payload retrieve_random_output_payload()
    {
        if (output_buffer.size() > 0)
        {
            int rand_index = rand() % output_buffer.size();
            Payload returned = output_buffer[rand_index];
            output_buffer.erase(output_buffer.begin() + rand_index);
            return returned;
        }
        else
        {
            return Payload::NONE();
        }
    }

    void update_progress(float delta)
    {
        switch (kind)
        {
            case Kind::Storage:
            {
                if (input_buffer.size() > 0)
                {
                    for (size_t i = 0; i < input_buffer.size(); i++)
                    {
                        output_buffer.push_back(input_buffer[i]);
                    }
                    input_buffer.clear();
                    progress.clear();
                }
            } break;

            case Kind::Transmuter:
            {
                for (size_t i = 0; i < input_buffer.size();)
                {
                    progress[i] += delta * rate;
                    if (progress[i] > 1.0f)
                    {
                        Payload payload = input_buffer[i];
                        input_buffer.erase(input_buffer.begin() + i);
                        progress.erase(progress.begin() + i);
                        payload.transmute();
                        output_buffer.push_back(payload);
                    }
                    else
                    {
                        i++;
                    }
                }
            } break;

            case Kind::NONE:
            case Kind::COUNT:
                break;
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
        carried_payload = nodes[which_node].retrieve_random_output_payload();
    }

    void finish_delivery(std::vector<Node> &nodes)
    {
        if (!carried_payload.is_none())
        {
            size_t node_index;
            if (!travelling_from_b) node_index = node_b;
            else node_index = node_a;
            nodes[node_index].add_payload_to_input_buffer(carried_payload.kind);
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
        size_t node_i = 0;
        for (auto node_it = nodes.begin(); node_it != nodes.end(); node_it++, node_i++)
        {
            if (node_it->is_window_open)
            {
                ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

                char window_name_buf[STR_BUF_SMALL];
                snprintf(window_name_buf, sizeof(window_name_buf), "Node: %s###Node%zu", node_it->name_buf, node_i);

                if (ImGui::Begin(window_name_buf, &node_it->is_window_open))
                {
                    ImGui::InputText("Name", node_it->name_buf, sizeof(node_it->name_buf));

                    // ImGui::BulletText("Kind: %s", node_it->get_kind_str());
                    if (ImGui::BeginCombo("Kind", node_it->get_kind_str(), 0))
                    {
                        for (int i = 1; i < (int)Node::Kind::COUNT; i++)
                        {
                            const bool is_selected = (Node::Kind)i == node_it->kind;
                            if (ImGui::Selectable(Node::get_kind_str((Node::Kind)i), is_selected))
                            {
                                node_it->kind = (Node::Kind)i;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Text("Input buffer:");
                    for (size_t input_i = 0; input_i < node_it->input_buffer.size(); input_i++)
                    {
                        ImGui::BulletText("%s. Progress: %.2f",
                            node_it->input_buffer[input_i].get_kind_string(),
                            node_it->progress[input_i] * 100.0f);
                    }

                    ImGui::Text("Output buffer:");
                    for (auto input_it = node_it->output_buffer.begin(); input_it != node_it->output_buffer.end(); input_it++)
                    {
                        ImGui::BulletText("%s", input_it->get_kind_string());
                    }

                    ImGui::End();
                }
            }
        }
    }
};
