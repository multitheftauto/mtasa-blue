/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CAspectRatioConverter.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

//
// Convert horizontal positions of DX/GUI items designed for one aspect ratio
// to align with GTA HUD elements scaled to another aspect ratio.
//
class CAspectRatioConverter
{
public:
    ZERO_ON_NEW

    // Call whenever viewport width changes
    void Init(float viewportWidth)
    {
        m_viewportWidth = viewportWidth;
        m_halfViewportWidth = viewportWidth * 0.5f;
        UpdateConversion();
    }

    // Call once per frame to update target HUD aspect ratio
    void Pulse(float targetRatioValue)
    {
        m_targetRatioValue = targetRatioValue;
        UpdateConversion();
    }

    // Call when source aspect ratio changes (0 disables conversion)
    void SetSourceRatioValue(float sourceRatioValue)
    {
        m_sourceRatioValue = sourceRatioValue;
        UpdateConversion();
    }

    float GetSourceRatioValue() const { return m_sourceRatioValue; }

    // Temp suspend conversion (e.g. when F11 map is displayed)
    void SetSuspended(bool suspended) { m_suspended = suspended; }

    bool IsEnabled() const { return m_validRatios && !m_suspended; }

    // Keep exact screen center fixed; scale other X coordinates relative to nearest horizontal edge
    float ConvertPositionForAspectRatio(const float x) const
    {
        if (!IsEnabled() || x == m_halfViewportWidth)
            return x;

        if (x < m_halfViewportWidth)
            return x * m_convertScale;

        return m_viewportWidth - (m_viewportWidth - x) * m_convertScale;
    }

    // Convert a horizontal screen position and width (in-place)
    void ConvertSideForAspectRatio(float* x, float* width) const
    {
        if (!IsEnabled())
            return;

        // Anchor via rect center so elements straddling screen midpoint do not invert
        const float center = *x + (*width) * 0.5f;
        const float newCenter = ConvertPositionForAspectRatio(center);
        *width *= m_convertScale;
        *x = newCenter - (*width) * 0.5f;
    }

protected:
    void UpdateConversion()
    {
        m_validRatios = (m_sourceRatioValue > 0.0f) && (m_targetRatioValue > 0.0f) && (m_viewportWidth > 0.0f);
        m_convertScale = m_validRatios ? (m_sourceRatioValue / m_targetRatioValue) : 1.0f;
    }

    bool  m_validRatios = false;
    bool  m_suspended = false;
    float m_sourceRatioValue = 0.0f;
    float m_targetRatioValue = 0.0f;
    float m_viewportWidth = 0.0f;
    float m_halfViewportWidth = 0.0f;
    float m_convertScale = 1.0f;
};
