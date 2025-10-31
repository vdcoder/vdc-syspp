# Deploying Metal Server to Railway

This guide walks you through deploying the Metal full-stack application to Railway.

## Prerequisites

- GitHub account
- Railway account (sign up at https://railway.app)
- Your code pushed to a GitHub repository

## Quick Deploy

### Option 1: Railway Web Interface

1. **Go to Railway**: https://railway.app
2. **Login** with your GitHub account
3. **Create New Project** → "Deploy from GitHub repo"
4. **Select your repository**: `vdcoder/vdc-syspp`
5. **Select the service root**: Choose `metal/` as the root directory
6. **Railway will auto-detect** the Dockerfile and start building
7. **Wait for deployment** (first build takes ~5-10 minutes due to Emscripten installation)
8. **Get your URL**: Railway provides a public URL automatically

### Option 2: Railway CLI

```bash
# Install Railway CLI
npm install -g @railway/cli

# Login
railway login

# Navigate to your project
cd metal/

# Initialize Railway project
railway init

# Deploy
railway up

# Open in browser
railway open
```

## Configuration

Railway automatically sets the `PORT` environment variable. The Metal Server reads this and binds to the correct port.

### Environment Variables (Optional)

You can set these in the Railway dashboard under **Variables**:

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | Set by Railway | Server port (auto-configured) |
| `STATIC_PATH` | `./static` | Path to static files (default works) |

## Build Process

The Dockerfile performs these steps:

1. **Builder Stage**:
   - Installs build tools (cmake, gcc, etc.)
   - Installs Emscripten for WebAssembly compilation
   - Builds oatpp framework from source
   - Compiles WebAssembly client
   - Builds C++ server in Release mode

2. **Production Stage**:
   - Creates minimal Ubuntu image
   - Copies only the built binary
   - Copies static files (client app)
   - Copies required shared libraries

**Build Time**: ~5-10 minutes for first build, ~2-3 minutes for subsequent builds (with caching)

## Monitoring

### Check Deployment Status

In Railway dashboard:
- **Deployments** tab shows build logs
- **Metrics** tab shows resource usage
- **Logs** tab shows runtime logs

### Health Check

Once deployed, test your endpoints:

```bash
# Replace YOUR-APP.up.railway.app with your actual URL
curl https://YOUR-APP.up.railway.app/health

# Should return:
# {
#   "status": "healthy",
#   "timestamp": "..."
# }
```

### Test the Client

Open `https://YOUR-APP.up.railway.app` in your browser to see the WebAssembly client.

## Troubleshooting

### Build Fails at Emscripten Step

**Error**: `Permission denied` on `build.sh`

**Solution**: Already fixed in Dockerfile with `chmod +x build.sh`

### Build Timeout

**Issue**: Railway has build time limits

**Solution**: The build should complete within limits. If it times out:
- Check Railway dashboard for specific error
- Ensure `.dockerignore` excludes `build/` and `static/` directories
- First build takes longer (~10 min) due to Emscripten installation

### Runtime Errors

**Check logs**:
```bash
railway logs
```

**Common issues**:
- Port binding: Railway sets PORT automatically, server should listen on it
- Missing static files: Verify Dockerfile copies `static/` directory
- Library issues: Ensure oatpp shared libraries are copied in Dockerfile

### Server Won't Start

**Error**: `Couldn't bind. Address already in use`

**Solution**: Don't set PORT environment variable manually. Railway sets it automatically.

**Error**: `Static files not found`

**Solution**: Check Dockerfile copies files correctly:
```dockerfile
COPY --from=builder /app/static /app/static
```

## Custom Domain (Optional)

1. Go to **Settings** in Railway dashboard
2. Add your custom domain
3. Configure DNS records as shown
4. Railway provides automatic HTTPS

## Costs

Railway pricing (as of 2025):
- **Free Tier**: $5 worth of usage credits per month
- **Pro Plan**: $20/month includes $20 credits
- **Usage**: Billed per hour of compute time

Metal Server should fit comfortably in free tier for development/testing.

## Updating Your Deployment

### Automatic Deploys

Railway automatically redeploys when you push to GitHub:

```bash
git add .
git commit -m "Update feature"
git push origin main
```

Railway detects the push and triggers a new build.

### Manual Deploy

Using Railway CLI:
```bash
railway up
```

## Next Steps

- Set up custom domain
- Configure environment variables for production
- Set up monitoring and alerts
- Add database if needed (Railway supports PostgreSQL, Redis, etc.)
- Enable auto-scaling if traffic increases

## Support

- **Railway Docs**: https://docs.railway.app
- **Railway Discord**: https://discord.gg/railway
- **Metal Server Issues**: https://github.com/vdcoder/vdc-syspp/issues

## Production Checklist

Before going to production:

- [ ] Build completes successfully
- [ ] Health endpoint responds
- [ ] Client loads in browser
- [ ] WebAssembly module loads and runs
- [ ] API endpoints work correctly
- [ ] HTTPS is enabled (automatic with Railway)
- [ ] Logs are clean (no errors)
- [ ] Custom domain configured (optional)
- [ ] Monitoring set up
- [ ] Backups configured if using database

---

**Ready to deploy?** Push your code to GitHub and follow Option 1 above! 🚀
