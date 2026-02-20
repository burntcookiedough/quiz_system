/**
 * Structured Canvas - Common UI Components
 * Shared behavioral logic across different frontend pages.
 */

// Simple SVG Line Ploter for the Dashboard Graph
function plotSvgGraph(svgSelector, values) {
    const svg = document.querySelector(svgSelector);
    if (!svg || !values || values.length === 0) return;

    // Reset old elements except grid lines and texts
    svg.querySelectorAll('.data-area, .data-line, .data-point').forEach(el => el.remove());

    const width = 600;
    const height = 200;
    const paddingY = 20; // top padding
    const paddingYBottom = 40; // bottom padding for text (160 is base)
    const effectiveHeight = height - paddingY - paddingYBottom;

    // Find min/max for scaling
    const maxVal = Math.max(...values, 10); // at least 10 scale

    const stepX = width / (values.length - 1);

    const pointsToPixels = values.map((val, index) => {
        const x = index * stepX;
        // Invert Y because SVG origin (0,0) is top-left
        const scale = val / maxVal;
        const y = paddingY + effectiveHeight - (scale * effectiveHeight);
        return { x, y };
    });

    const dataLineString = pointsToPixels.map(p => `${p.x},${p.y}`).join(' ');
    const dataAreaString = `0,200 ${dataLineString} ${width},200`;

    // 1. Plot Area
    const polygon = document.createElementNS("http://www.w3.org/2000/svg", "polygon");
    polygon.setAttribute("points", dataAreaString);
    polygon.setAttribute("class", "data-area");
    svg.appendChild(polygon);

    // 2. Plot Line
    const polyline = document.createElementNS("http://www.w3.org/2000/svg", "polyline");
    polyline.setAttribute("points", dataLineString);
    polyline.setAttribute("class", "data-line");
    svg.appendChild(polyline);

    // 3. Plot Points
    pointsToPixels.forEach(p => {
        const circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
        circle.setAttribute("cx", p.x);
        circle.setAttribute("cy", p.y);
        circle.setAttribute("r", 5);
        circle.setAttribute("class", "data-point");
        svg.appendChild(circle);
    });
}

window.StructComponents = {
    plotSvgGraph
};
