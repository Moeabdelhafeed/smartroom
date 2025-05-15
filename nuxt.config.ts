import tailwindcss from "@tailwindcss/vite";

// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  compatibilityDate: '2024-11-01',
  devtools: { enabled: true },
  vite:{
    plugins: [tailwindcss()]
  },
  css: ["~/assets/css/tailwind.css"],
  ssr: true,
  nitro: {
    preset: 'static' // This makes Nuxt generate a static site
  },
  modules: ['@pinia/nuxt', "nuxt-bootstrap-icons"],
  app:{
    baseURL: '/smartroom/',
    buildAssetsDir: 'assets',
    head:{
      link:[
        { rel: 'stylesheet', href: 'https://cdn.jsdelivr.net/npm/bootstrap-icons@1.13.1/font/bootstrap-icons.min.css' },
          { rel: 'icon', type: 'image/x-icon', href: '/smartroom/favicon.ico' }
      ],
      title: 'Smart Room',
    }
  }
})