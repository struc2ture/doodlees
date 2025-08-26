#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <imgui.h>

#include "types.hpp"

#include "gl_tiles.cpp"
#include "util.hpp"

const char *AGENT_NAMES[] =
{
    "Humgef",
    "Haliser",
    "Kierty",
    "Giolist",
    "Leemper"
};

static const char *_get_random_agent_name()
{
    int i = rand() % array_size(AGENT_NAMES);
    return AGENT_NAMES[i];
}

const char *NODE_NAMES[] =
{
    "Alpha",
    "Beta",
    "Gamma",
    "Delta",
    "Epsilon",
    "Zeta"
};

static const char *_get_random_node_name()
{
    int i = rand() % array_size(NODE_NAMES);
    return NODE_NAMES[i];
}

struct Node
{
    char name_buf[STR_BUF_SMALL];
    bool is_window_open = false;

    int payloads_received = 0;

    Node(const char *name)
    {
        strcpy(this->name_buf, name);
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

    Agent(const char *name)
    {
        strcpy(this->name_buf, name);
    }

    inline bool destinations_valid()
    {
        return node_a > 0 && node_b > 0 && node_a != node_b;
    }

    void update_progress(std::vector<Node> *nodes, float delta)
    {
        if (destinations_valid())
        {
            progress += delta * progress_rate;
        }
        if (progress > 1.0f)
        {
            if (!travelling_from_b)
            {
                (*nodes)[node_b].payloads_received++;
            }
            else
            {
                (*nodes)[node_a].payloads_received++;
            }
            travelling_from_b = !travelling_from_b;
            progress = 0.0f;
        }
    }
};

struct Game
{
    std::vector<Agent> agents;
    std::vector<Node> nodes;

    char agent_list_name_edit_buf[STR_BUF_SMALL];
    char node_list_name_edit_buf[STR_BUF_SMALL];

    void init()
    {
        srand(0);
        agents.push_back(Agent("NONE"));
        nodes.push_back(Node("NONE"));
        strcpy(agent_list_name_edit_buf, _get_random_agent_name());
        strcpy(node_list_name_edit_buf, _get_random_node_name());
    }

    void frame(float delta)
    {
        for (size_t i = 1; i < agents.size(); i++)
        {
            agents[i].update_progress(&nodes, delta);
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
            strcpy(agent_list_name_edit_buf, _get_random_agent_name());
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
                    }
                    ImGui::BulletText("Progress: %.3f", agents[i].progress);

                    ImGui::End();
                }
            }
        }
    }

    void draw_node_list_window()
    {
        ImGui::Begin("Nodes");

        ImGui::InputText("Name", node_list_name_edit_buf, sizeof(node_list_name_edit_buf));
        ImGui::SameLine();
        if (ImGui::Button("Add"))
        {
            nodes.push_back(Node(node_list_name_edit_buf));
            strcpy(node_list_name_edit_buf, _get_random_node_name());
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
        for (size_t i = 1; i < nodes.size(); i++)
        {
            if (nodes[i].is_window_open)
            {
                ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

                char window_name_buf[STR_BUF_SMALL];
                snprintf(window_name_buf, sizeof(window_name_buf), "Node: %s###Node%zu", nodes[i].name_buf, i);

                if (ImGui::Begin(window_name_buf, &nodes[i].is_window_open))
                {
                    ImGui::InputText("Name", nodes[i].name_buf, sizeof(nodes[i].name_buf));

                    ImGui::BulletText("Payloads received: %d", nodes[i].payloads_received);

                    ImGui::End();
                }
            }
        }
    }
};
