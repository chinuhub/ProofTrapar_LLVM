; ModuleID = 'ticket_new.c'
source_filename = "ticket_new.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@lock = dso_local global i32 0, align 4
@next_ticket = dso_local global i32 0, align 4
@now_serving = dso_local global i32 0, align 4
@c = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  br label %3

3:                                                ; preds = %7, %0
  %4 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %5 = extractvalue { i32, i1 } %4, 1
  %6 = xor i1 %5, true
  br i1 %6, label %7, label %8

7:                                                ; preds = %3
  br label %3

8:                                                ; preds = %3
  store i32 0, i32* %2, align 4
  %9 = load i32, i32* @next_ticket, align 4
  %10 = add nsw i32 %9, 1
  %11 = srem i32 %10, 3
  %12 = load i32, i32* @now_serving, align 4
  %13 = icmp eq i32 %11, %12
  br i1 %13, label %14, label %15

14:                                               ; preds = %8
  store i32 3, i32* %2, align 4
  br label %20

15:                                               ; preds = %8
  %16 = load i32, i32* @next_ticket, align 4
  store i32 %16, i32* %2, align 4
  %17 = load i32, i32* @next_ticket, align 4
  %18 = add nsw i32 %17, 1
  %19 = srem i32 %18, 3
  store i32 %19, i32* @next_ticket, align 4
  br label %20

20:                                               ; preds = %15, %14
  store i32 0, i32* @lock, align 4
  %21 = load i32, i32* %2, align 4
  store i32 %21, i32* %1, align 4
  %22 = load i32, i32* %1, align 4
  %23 = icmp eq i32 %22, 3
  br i1 %23, label %24, label %25

24:                                               ; preds = %20
  br label %33

25:                                               ; preds = %20
  br label %26

26:                                               ; preds = %25, %31
  %27 = load i32, i32* @now_serving, align 4
  %28 = load i32, i32* %1, align 4
  %29 = icmp eq i32 %27, %28
  br i1 %29, label %30, label %31

30:                                               ; preds = %26
  br label %32

31:                                               ; preds = %26
  br label %26

32:                                               ; preds = %30
  br label %33

33:                                               ; preds = %32, %24
  %34 = load i32, i32* @c, align 4
  %35 = add nsw i32 %34, 1
  store i32 %35, i32* @c, align 4
  %36 = load i32, i32* @c, align 4
  %37 = icmp eq i32 %36, 1
  br i1 %37, label %39, label %38

38:                                               ; preds = %33
  call void @__ASSERT()
  br label %39

39:                                               ; preds = %38, %33
  %40 = load i32, i32* @c, align 4
  %41 = add nsw i32 %40, -1
  store i32 %41, i32* @c, align 4
  %42 = load i32, i32* @now_serving, align 4
  %43 = add nsw i32 %42, 1
  store i32 %43, i32* @now_serving, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  br label %3

3:                                                ; preds = %7, %0
  %4 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %5 = extractvalue { i32, i1 } %4, 1
  %6 = xor i1 %5, true
  br i1 %6, label %7, label %8

7:                                                ; preds = %3
  br label %3

8:                                                ; preds = %3
  %9 = load i32, i32* @next_ticket, align 4
  %10 = add nsw i32 %9, 1
  %11 = srem i32 %10, 3
  %12 = load i32, i32* @now_serving, align 4
  %13 = icmp eq i32 %11, %12
  br i1 %13, label %14, label %15

14:                                               ; preds = %8
  store i32 3, i32* %2, align 4
  br label %20

15:                                               ; preds = %8
  %16 = load i32, i32* @next_ticket, align 4
  store i32 %16, i32* %2, align 4
  %17 = load i32, i32* @next_ticket, align 4
  %18 = add nsw i32 %17, 1
  %19 = srem i32 %18, 3
  store i32 %19, i32* @next_ticket, align 4
  br label %20

20:                                               ; preds = %15, %14
  store i32 0, i32* @lock, align 4
  %21 = load i32, i32* %2, align 4
  store i32 %21, i32* %1, align 4
  %22 = load i32, i32* %1, align 4
  %23 = icmp eq i32 %22, 3
  br i1 %23, label %24, label %25

24:                                               ; preds = %20
  br label %33

25:                                               ; preds = %20
  br label %26

26:                                               ; preds = %25, %31
  %27 = load i32, i32* @now_serving, align 4
  %28 = load i32, i32* %1, align 4
  %29 = icmp eq i32 %27, %28
  br i1 %29, label %30, label %31

30:                                               ; preds = %26
  br label %32

31:                                               ; preds = %26
  br label %26

32:                                               ; preds = %30
  br label %33

33:                                               ; preds = %32, %24
  %34 = load i32, i32* @c, align 4
  %35 = add nsw i32 %34, 1
  store i32 %35, i32* @c, align 4
  %36 = load i32, i32* @c, align 4
  %37 = icmp eq i32 %36, 1
  br i1 %37, label %39, label %38

38:                                               ; preds = %33
  call void @__ASSERT()
  br label %39

39:                                               ; preds = %38, %33
  %40 = load i32, i32* @c, align 4
  %41 = add nsw i32 %40, -1
  store i32 %41, i32* @c, align 4
  %42 = load i32, i32* @now_serving, align 4
  %43 = add nsw i32 %42, 1
  store i32 %43, i32* @now_serving, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
