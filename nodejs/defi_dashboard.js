/**
 * @file
 * @brief DeFi dashboard - TVL, APY, and protocol metrics
 * @author PARTH
 * @date 2026-02-10
 */

const express = require('express');
const axios = require('axios');

// Configuration
const PORT = process.env.PORT || 3000;
const PROTOCOLS = [
    {
        name: 'Aave',
        type: 'Lending',
        tvl: 12000000000, // $12B
        apy: 4.2,
        category: 'DeFi Lending'
    },
    {
        name: 'Uniswap V3',
        type: 'DEX',
        tvl: 8500000000, // $8.5B
        apy: 8.5,
        category: 'Decentralized Exchange'
    },
    {
        name: 'Lido',
        type: 'Staking',
        tvl: 28000000000, // $28B
        apy: 5.8,
        category: 'Liquid Staking'
    },
    {
        name: 'Curve',
        type: 'DEX',
        tvl: 4200000000, // $4.2B
        apy: 6.3,
        category: 'Automated Market Maker'
    },
    {
        name: 'MakerDAO',
        type: 'Lending',
        tvl: 7800000000, // $7.8B
        apy: 2.8,
        category: 'DeFi Lending'
    }
];

// In-memory data store
let protocolHistory = [];

/**
 * @brief Calculate DeFi metrics
 * @param protocols List of protocol data
 * @return Total TVL, average APY, top protocols
 */
function calculateMetrics(protocols) {
    const totalTVL = protocols.reduce((sum, p) => sum + p.tvl, 0);
    const avgAPY = protocols.reduce((sum, p) => sum + p.apy, 0) / protocols.length;

    const sortedByTVL = [...protocols].sort((a, b) => b.tvl - a.tvl);
    const topByAPY = [...protocols].sort((a, b) => b.apy - a.apy);

    return {
        totalTVL,
        avgAPY,
        topTVL: sortedByTVL.slice(0, 5),
        topAPY: topByAPY.slice(0, 5),
        totalProtocols: protocols.length
    };
}

/**
 * @brief Format currency
 * @param amount Amount in dollars
 * @return Formatted string
 */
function formatCurrency(amount) {
    if (amount >= 1000000000) {
        return `$${(amount / 1000000000).toFixed(1)}B`;
    } else if (amount >= 1000000) {
        return `$${(amount / 1000000).toFixed(1)}M`;
    } else if (amount >= 1000) {
        return `$${(amount / 1000).toFixed(1)}K`;
    } else {
        return `$${amount.toFixed(2)}`;
    }
}

/**
 * @brief Express middleware for CORS
 */
const corsMiddleware = (req, res, next) => {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    next();
};

// Express app
const app = express();

app.use(corsMiddleware);
app.use(express.json());

/**
 * @route GET /api/protocols
 * @brief Get all protocols with their metrics
 */
app.get('/api/protocols', (req, res) => {
    const metrics = calculateMetrics(PROTOCOLS);

    res.json({
        success: true,
        timestamp: new Date().toISOString(),
        metrics,
        protocols: PROTOCOLS.map(p => ({
            name: p.name,
            tvl: p.tvl,
            tvlFormatted: formatCurrency(p.tvl),
            apy: p.apy,
            category: p.category
        }))
    });
});

/**
 * @route GET /api/protocols/:name
 * @brief Get specific protocol details
 */
app.get('/api/protocols/:name', (req, res) => {
    const protocolName = req.params.name;
    const protocol = PROTOCOLS.find(p => p.name.toLowerCase() === protocolName.toLowerCase());

    if (!protocol) {
        return res.status(404).json({
            success: false,
            error: `Protocol ${protocolName} not found`
        });
    }

    res.json({
        success: true,
        protocol: {
            name: protocol.name,
            type: protocol.type,
            tvl: protocol.tvl,
            tvlFormatted: formatCurrency(protocol.tvl),
            apy: protocol.apy,
            category: protocol.category
        }
    });
});

/**
 * @route GET /api/metrics
 * @brief Get aggregated DeFi metrics
 */
app.get('/api/metrics', (req, res) => {
    const metrics = calculateMetrics(PROTOCOLS);

    res.json({
        success: true,
        timestamp: new Date().toISOString(),
        summary: {
            totalTVL: formatCurrency(metrics.totalTVL),
            averageAPY: `${metrics.avgAPY.toFixed(2)}%`,
            totalProtocols: metrics.totalProtocols,
            marketDominance: 'fragmented but growing'
        },
        topByTVL: metrics.topTVL.map(p => ({
            name: p.name,
            tvlFormatted: formatCurrency(p.tvl),
            marketShare: `${((p.tvl / metrics.totalTVL) * 100).toFixed(1)}%`
        })),
        topByAPY: metrics.topAPY.map(p => ({
            name: p.name,
            apy: `${p.apy.toFixed(2)}%`,
            category: p.category
        }))
    });
});

/**
 * @route GET /api/history
 * @brief Get protocol history (in-memory)
 */
app.get('/api/history', (req, res) => {
    res.json({
        success: true,
        timestamp: new Date().toISOString(),
        history: protocolHistory,
        count: protocolHistory.length
    });
});

/**
 * @route POST /api/simulate/update
 * @brief Simulate protocol TVL/APY updates (for demo)
 */
app.post('/api/simulate/update', (req, res) => {
    const { protocolName, tvlChange, apyChange } = req.body;

    const protocolIndex = PROTOCOLS.findIndex(p =>
        p.name.toLowerCase() === protocolName.toLowerCase()
    );

    if (protocolIndex === -1) {
        return res.status(404).json({
            success: false,
            error: `Protocol ${protocolName} not found`
        });
    }

    // Update protocol data (in-memory, won't persist)
    PROTOCOLS[protocolIndex].tvl = PROTOCOLS[protocolIndex].tvl + (tvlChange || 0);
    PROTOCOLS[protocolIndex].apy = PROTOCOLS[protocolIndex].apy + (apyChange || 0);

    // Add to history
    protocolHistory.push({
        timestamp: new Date().toISOString(),
        protocol: PROTOCOLS[protocolIndex].name,
        action: 'Simulated Update',
        tvlChange: tvlChange || 0,
        apyChange: apyChange || 0
    });

    const updatedMetrics = calculateMetrics(PROTOCOLS);

    res.json({
        success: true,
        message: 'Protocol updated successfully (simulated)',
        updatedProtocol: {
            name: PROTOCOLS[protocolIndex].name,
            newTVL: formatCurrency(PROTOCOLS[protocolIndex].tvl),
            newAPY: `${PROTOCOLS[protocolIndex].apy.toFixed(2)}%`
        },
        newMetrics: {
            totalTVL: formatCurrency(updatedMetrics.totalTVL),
            averageAPY: `${updatedMetrics.avgAPY.toFixed(2)}%`
        }
    });
});

/**
 * @route GET /
 * @brief Dashboard UI
 */
app.get('/', (req, res) => {
    const metrics = calculateMetrics(PROTOCOLS);

    res.send(`
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>DeFi Dashboard</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #ffffff;
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        .header {
            text-align: center;
            margin-bottom: 30px;
        }
        .header h1 {
            font-size: 2.5rem;
            margin: 0 0 10px 0;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .header p {
            font-size: 1.2rem;
            opacity: 0.9;
        }
        .dashboard {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .card {
            background: rgba(255, 255, 255, 0.1);
            border-radius: 15px;
            padding: 25px;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.2);
        }
        .card h2 {
            margin: 0 0 15px 0;
            font-size: 1.5rem;
            color: #64b5f6;
        }
        .stat-value {
            font-size: 2.5rem;
            font-weight: bold;
            margin: 10px 0;
        }
        .stat-label {
            font-size: 0.9rem;
            opacity: 0.8;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .protocol-table {
            width: 100%;
            border-collapse: collapse;
        }
        .protocol-table th,
        .protocol-table td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
        .protocol-table th {
            background: rgba(100, 100, 100, 0.1);
            font-weight: 600;
        }
        .apy-high {
            color: #00c853;
        }
        .apy-medium {
            color: #ffc107;
        }
        .apy-low {
            color: #dc3545;
        }
        .footer {
            text-align: center;
            margin-top: 40px;
            opacity: 0.7;
            font-size: 0.9rem;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>⚡ DeFi Dashboard</h1>
            <p>Real-time protocol metrics and analytics</p>
        </div>

        <div class="dashboard">
            <div class="card">
                <h2>Total Value Locked (TVL)</h2>
                <div class="stat-value">${(metrics.totalTVL / 1000000000).toFixed(1)}B</div>
                <div class="stat-label">Across all protocols</div>
            </div>

            <div class="card">
                <h2>Average APY</h2>
                <div class="stat-value">${metrics.avgAPY.toFixed(2)}%</div>
                <div class="stat-label">Weighted average yield</div>
            </div>

            <div class="card">
                <h2>Top Protocols by TVL</h2>
                <table class="protocol-table">
                    <thead>
                        <tr>
                            <th>Protocol</th>
                            <th>TVL</th>
                            <th>Market Share</th>
                        </tr>
                    </thead>
                    <tbody>
                        ${metrics.topTVL.map(p => `
                            <tr>
                                <td>${p.name}</td>
                                <td>${formatCurrency(p.tvl)}</td>
                                <td>${((p.tvl / metrics.totalTVL) * 100).toFixed(1)}%</td>
                            </tr>
                        `).join('')}
                    </tbody>
                </table>
            </div>

            <div class="card">
                <h2>Top Protocols by APY</h2>
                <table class="protocol-table">
                    <thead>
                        <tr>
                            <th>Protocol</th>
                            <th>APY</th>
                            <th>Category</th>
                        </tr>
                    </thead>
                    <tbody>
                        ${metrics.topAPY.map(p => `
                            <tr>
                                <td>${p.name}</td>
                                <td class="${p.apy > 5 ? 'apy-high' : p.apy > 3 ? 'apy-medium' : 'apy-low'}">${p.apy.toFixed(2)}%</td>
                                <td>${p.category}</td>
                            </tr>
                        `).join('')}
                    </tbody>
                </table>
            </div>
        </div>

        <div class="footer">
            <p>DeFi Dashboard v1.0 • Built with Express.js</p>
            <p>API endpoints available at /api/*</p>
        </div>
    </div>
</body>
</html>
    `);
});

// Health check endpoint
app.get('/health', (req, res) => {
    res.json({
        status: 'healthy',
        timestamp: new Date().toISOString(),
        uptime: process.uptime()
    });
});

// Start server
app.listen(PORT, () => {
    console.log('='.repeat(50));
    console.log('DeFi Dashboard Server');
    console.log('Author: PARTH');
    console.log(`Date: ${new Date().toLocaleDateString()}`);
    console.log('='.repeat(50));
    console.log(`Server running on http://localhost:${PORT}`);
    console.log(`Health check: http://localhost:${PORT}/health`);
    console.log(`API endpoints:`);
    console.log(`  - GET  /api/protocols`);
    console.log(`  - GET  /api/protocols/:name`);
    console.log(`  - GET  /api/metrics`);
    console.log(`  - GET  /api/history`);
    console.log(`  - POST /api/simulate/update`);
    console.log('='.repeat(50));
});

module.exports = app;
