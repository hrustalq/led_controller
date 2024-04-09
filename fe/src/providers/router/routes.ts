import { RouteRecordRaw } from "vue-router";

export const routes: RouteRecordRaw[] = [
  {
    name: "Главная",
    path: "/",
    component: () => import("../../pages/home-page.vue")
  },
  {
    name: "Контроллеры",
    path: "/controllers",
    component: () => import("../../pages/controllers-page.vue"),
  },
];
